//
//  main.cpp
//  Swarm Natural Selection
//
//  Created by Cody Nguyen  on 2019-05-30.
//  Copyright © 2019 Cody Nguyen . All rights reserved.
//

//srand(time(NULL))

#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
//#include <SDL2_mixer/SDL_mixer.h>
#include <string>
#include <time.h>
#include <cmath>

using namespace std;
//Screen attributes
const int SCREEN_WIDTH  = 1020;
const int SCREEN_HEIGHT = 760;

//Set size of button width and height and number of button for later use
const int BUTTON_WIDTH = 312;
const int BUTTON_HEIGHT = 157;
const int TOTAL_BUTTONS = 1;

/*
 //Set size of mute button width and height and number of mute button for later use
 const int MBUTTON_WIDTH = 53;
 const int MBUTTON_HEIGHT = 53;
 const int MTOTAL_BUTTONS = 1;
 */

//Set the values for different sprites
//If mouse outside of button
//If mouse over buttons
//And if mouse clicks on button
enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_TOTAL = 3
};

/*
 //Set values for different mute sprites
 //if mouse is out of button or over button for both the unmute button and mute button (and click)
 enum LButtonSpriteM
 {
 MBUTTON_SPRITE_MOUSE_OUT = 0,
 MBUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
 MBUTTON_SPRITE_MOUSE_DOWN = 2,
 UMBUTTON_SPRITE_MOUSE_OUT = 3,
 UMBUTTON_SPRITE_MOUSE_OVER_MOTION = 4,
 UBUTTON_SPRITE_MOUSE_DOWN = 5,
 MBUTTON_SPRITE_TOTAL = 6
 };
 */

//Creates a variable the holds values for a circle
struct Circle
{
    int x, y;
    int r;
};

//Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture();
    
    //Deallocates memory
    ~LTexture();
    
    //Loads image at specified path
    bool loadFromFile( string path );
    
    //Deallocates texture
    void free();
    
    //Renders texture at given point
    void render( int x, int y );
    
    //Gets image dimensions
    int getWidth();
    int getHeight();
    
private:
    //The actual hardware texture
    SDL_Texture* mTexture;
    
    //Image dimensions
    int mWidth;
    int mHeight;
};

class mushroomSoldier
{
public:
    
    //axis velocity of soldier
    //static const int Vel = 10;
    
    static const int radius = 8;
    //Intializes the variables
    mushroomSoldier(int x, int y);
    
    //moves the soldier
    bool move(SDL_Rect& square, double mVel, double mAngle, int maxSoldiers, int selected, int Neg, mushroomSoldier* army[]);
    
    //renders the soldier
    void render();
    
    Circle& getCollider();
    
private:
    int mPosX, mPosY;
    
    int mVelX, mVelY;
    
    //Dot's collision box
    Circle mCollider;
    
    //Moves the collision circle
    void shiftCollider();
    
    LTexture *mTexture;
};

class mushroomArmy
{
public:
    //number of mushroomSoldiers
    static const int maxSoldiers = 50;
    
    //Maximum axis velocity of the dot
    static const int armyVel = 7;
    
    //Initializes the army
    mushroomArmy();
    
    //Deallocates the army
    ~mushroomArmy();
    
    void handleEvent( SDL_Event& e );

    //moves the army
    void move(SDL_Rect& square);
    
    //shows the mushroom soldiers
    void renderArmy();
    
private:
    //the mushroom Soldiers
    mushroomSoldier* mushroomSoldiers [maxSoldiers];
    
    //The X and Y offsets of the dot
    double mPosX, mPosY;
    
    //The velocity of the dot
    double mVelX, mVelY, mVel, mAngle;
    
    //Moves the colliders of soldiers in the army
    void shiftArmyColliders();
    
    //Texture shared by all the soldiers
    LTexture armyTexture;
    
    //The colliders of all the army's Soldiers
    Circle armyColliders[maxSoldiers];
};

//The mouse button (play button)
class LButton
{
public:
    //Initializes internal variables
    LButton();
    
    //Sets top left position
    void setPosition( int x, int y );
    
    //Handles mouse event
    void handleEvent( SDL_Event* e );
    
    //Shows button sprite
    void render();
    
private:
    //Top left position
    SDL_Point mPosition;
    
    //Currently used global sprite
    LButtonSprite mCurrentSprite;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void closeMedia();

//calculate the distance squared between two points
float distanceSquared (int x1, int y1, int x2, int y2);

//Circle collision detector
bool checkCollision (Circle& a, Circle& b);

//Circle/Box collision detector
bool checkCollision( Circle& a, SDL_Rect& b );


//The window we'll be rendering to
SDL_Window* window = NULL;

//The window renderer
SDL_Renderer* renderer = NULL;

//Scene textures
LTexture blueshroomTex;
LTexture background;

//Mix_Music for music files (longer) while Mix_Chunk for effect files (short)
//Background music
//Mix_Music *gbackground = NULL;

//Sound effect for button click
// *gbutton = NULL;

//Sprites for main button (play)
SDL_Rect gSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gButtonSpriteSheetTexture;

//Buttons objects
LButton gButtons[ TOTAL_BUTTONS ];

LTexture::LTexture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}

bool LTexture::loadFromFile( string path )
{
    //Get rid of preexisting texture
    free();
    
    //The final texture
    SDL_Texture* newTexture = NULL;
    
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );
        
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    
    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render( int x, int y )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    SDL_RenderCopy( renderer, mTexture, NULL, &renderQuad );
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}


LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
    
    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition( int x, int y )
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent( SDL_Event* e )
{
    //If mouse event happened
    if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );
        
        //Check if mouse is in button
        bool inside = true;
        
        //Mouse left of the button
        if( x < mPosition.x )
        {
            inside = false;
        }
        //Mouse right of the button
        else if( x > mPosition.x + BUTTON_WIDTH )
        {
            inside = false;
        }
        //Mouse above the button
        else if( y < mPosition.y )
        {
            inside = false;
        }
        //Mouse below the button
        else if( y > mPosition.y + BUTTON_HEIGHT )
        {
            inside = false;
        }
        
        //If mouse is not inside the button show the section of sheet for darker button
        if( !inside )
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        
        //If mouse is inside button split into another 2 cases
        else
        {
            //Set mouse over sprite
            switch( e->type )
            {
                    //If it is over mouse show the lighter section of sheet image
                case SDL_MOUSEMOTION:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                    
                    //If it presses down quit sdl and begin cleanup
                case SDL_MOUSEBUTTONDOWN:
                    SDL_Quit();
                    break;
                    
            }
        }
    }
}

/*
void LButton::render()
{
    //Show current button sprite
    gButtonSpriteSheetTexture.render( mPosition.x, mPosition.y, &gSpriteClips[ mCurrentSprite ] );
}
*/

mushroomSoldier::mushroomSoldier(int x, int y)
{
    mPosX = x;
    mPosY = y;
    
    mCollider.r = radius;
    
    mVelX = 0;
    mVelY = 0;
    
    shiftCollider();
}

bool mushroomSoldier::move(SDL_Rect& square, double mVel, double mAngle, int maxSoliders, int selected, int Neg,mushroomSoldier* army[])
{
    double offsetX, offsetY, angle, newVel;
    int newNeg=1;
    //Move the soldier on the x-axis and y-axis
    mVelX = Neg*mVel*cos(mAngle);
    mVelY = Neg*mVel*sin(mAngle);
    mPosX = mPosX + mVelX;
    mPosY = mPosY + mVelY;
    shiftCollider();
    
    //check for collision against all the soldiers
    for (int i = 0; i < maxSoliders; i++)
    {
        if ((i != selected) and (checkCollision(mCollider,army[i]->mCollider)))
        {
            offsetX = army[i]->mCollider.x - mPosX;
            offsetY = army[i]->mCollider.y - mPosY;
            newVel = sqrt(offsetX*offsetX + offsetY*offsetY);
            
            if ((offsetY>0) and (offsetX == 0))
            {
                angle = M_PI/2;
            }
            else if ((offsetY<0) and (offsetX == 0))
            {
                angle = (3*M_PI)/2;
            }
            else
            {
                angle = atan(offsetY/offsetX);
                //check if x velocity is negative
                if (offsetX<0)
                {
                    newNeg = -1;
                }
            }
            if ((army[i]->move(square,newVel,angle,maxSoliders,i,newNeg,army)))
            {
                //reverse the effect of the move
                mPosX = mPosX - mVelX;
                mPosY = mPosY - mVelY;
                shiftCollider();
            }
        }
    }
    bool revert = false;
    //check for collision against the border or with objects
    if ( ((mPosX - mCollider.r) < 0) or ((mPosX + mCollider.r) > SCREEN_WIDTH) or (checkCollision(mCollider, square)))
    {
        //reverse the effect of the move
        mPosX = mPosX - mVelX;
        shiftCollider();
        revert = true;
    }
    //check for collision against the border or with objects
    if ( ((mPosY - mCollider.r) < 0) or ((mPosY + mCollider.r) > SCREEN_HEIGHT) or (checkCollision(mCollider, square)))
    {
        //reverse the effect of the move
        mPosY = mPosY - mVelY;
        shiftCollider();
        revert = true;
    }
    return revert;
}

void mushroomSoldier::render()
{
    blueshroomTex.render(mPosX-mCollider.r, mPosY-mCollider.r);
}

Circle& mushroomSoldier::getCollider()
{
    return mCollider;
}

void mushroomSoldier::shiftCollider()
{
    //move collider to where dot is
    mCollider.x = mPosX;
    mCollider.y = mPosY;
}

mushroomArmy::mushroomArmy()
{
    //Intializes optimum
    mPosX = 300;
    mPosY = 300;
    
    //Intializes the velocity
    mVelX = 0;
    mVelY = 0;
    mVel = armyVel;
    mAngle = 0;
    
    //Intializes soldiers in rows and columns
    if (maxSoldiers < 10)
    {
        for (int i = 0; i < maxSoldiers; ++i)
        {
            mushroomSoldiers[i] = new mushroomSoldier( mPosX + i*40, mPosY);
        }
    }
    else
    {
        for( int i = 0; i < ((maxSoldiers -(maxSoldiers%10))/10); ++i)
        {
            if (i > (maxSoldiers - (maxSoldiers%10)))
            {
                for ( int j = 0; j < maxSoldiers%10; i++)
                {
                    mushroomSoldiers[ i*10 + j ] = new mushroomSoldier( mPosX + j*40, mPosY + i*40);
                }
            }
            else
            {
                for ( int j = 0; j < 10;++j)
                {
                    mushroomSoldiers[ i*10 + j ] = new mushroomSoldier( mPosX + j*40, mPosY + i*40);
                }
            }
        }
    }
}

mushroomArmy::~mushroomArmy()
{
    //Delete soldiers
    for( int i = 0; i < maxSoldiers; ++i)
    {
        delete mushroomSoldiers[i];
    }
}

void mushroomArmy::handleEvent(SDL_Event &e)
{
    //if a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        mVel = armyVel;
        //get arrow key directions and adjust velocity
        switch (e.key.keysym.sym)
        {
            case SDLK_UP:
                mVelY = mVelY - armyVel;
                break;
            case SDLK_DOWN:
                mVelY = mVelY + armyVel;
                break;
            case SDLK_LEFT:
                mVelX = mVelX - armyVel;
                break;
            case SDLK_RIGHT:
                mVelX = mVelX + armyVel;
                break;
        }
    }
    //if a key was released
    else if ( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //get arrow key directions and adjust velocity
        switch (e.key.keysym.sym)
        {
            case SDLK_UP:
                mVelY = mVelY + armyVel;
                break;
            case SDLK_DOWN:
                mVelY = mVelY - armyVel;
                break;
            case SDLK_LEFT:
                mVelX = mVelX + armyVel;
                break;
            case SDLK_RIGHT:
                mVelX = mVelX - armyVel;
                break;
        }
    }
    //check if all the movement keys are released
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (!(state[SDL_SCANCODE_DOWN]) and !(state[SDL_SCANCODE_UP]) and !(state[SDL_SCANCODE_RIGHT]) and !(state[SDL_SCANCODE_LEFT]))
    {
        mVel = 0;
    }
}

//move the optimal point for the army
void mushroomArmy::move(SDL_Rect& square)
{
    mPosX += mVelX;
    
    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }
    
    //Move the dot up or downx
    mPosY += mVelY;
    
    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
    
    //calculate the angle of the army
    int goingLeft = 1;
    if ((mVelY>0) and (mVelX == 0))
    {
        mAngle = M_PI/2;
    }
    else if ((mVelY<0) and (mVelX == 0))
    {
        mAngle = (3*M_PI)/2;
    }
    else if((mVelY == 0) and (mVelX == 0))
    {
        mVel=0;
        mAngle = NULL;
    }
    else
    {
        mAngle = atan(mVelY/mVelX);
        //check if x velocity is negative
        if (mVelX<0)
        {
            goingLeft = -1;
        }
    }
    
    mushroomSoldiers[0]->move(square, mVel, mAngle, maxSoldiers, 0, goingLeft, mushroomSoldiers);
    shiftArmyColliders();
    //move each soldier in the army
    /*
    for (int i=0; i < maxSoldiers; i++)
    {
        mushroomSoldiers[i]->move(square, mVel, mAngle, maxSoldiers, i, goingLeft, mushroomSoldiers);
        shiftArmyColliders();
    }
     */
}

void mushroomArmy::renderArmy()
{
    //render each mushroom soldier
    for ( int i =0; i <  maxSoldiers; i++)
    {
        mushroomSoldiers[i]->render();
    }
    
    //render a box representing the optimum
    SDL_Rect box;
    box.x = mPosX;
    box.y = mPosY;
    box.w = 10;
    box.h = 10;
    
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
    SDL_RenderDrawRect( renderer, &box);
}

void mushroomArmy::shiftArmyColliders()
{
    for (int i=0; i < maxSoldiers; i++)
    {
        armyColliders[i] = mushroomSoldiers[i]->getCollider();
    }
}

bool init()
{
    //Initialization flag
    bool success = true;
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        
        //Create window
        window = SDL_CreateWindow( "Natural Section", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
            if( renderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
                
                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                    
                    //Initialize SDL_mixer and display in console if error present using
                    //the SDL built in func
                    //first is audi freq, sample format, hardware channels (2 for stereo), and
                    //last is sample size
                    /*
                    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                    {
                        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                        success = false;
                    }
                     */
                }
            }
        }
    }
    
    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;
    
    //main soldier texture
    if( !blueshroomTex.loadFromFile( "dot.bmp" ) )
    {
        printf( "Failed to image image!\n" );
        success = false;
    }
    
    //Load background texture
    if( !background.loadFromFile( "background.png" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    
    /*
    //Load music and display in conosle if error using an sdl func
    gbackground = Mix_LoadMUS( "background.wav" );
    if( gbackground == NULL )
    {
        printf( "Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    
    //Load sound effect for button and display in conosle if error using an sdl func
    gbutton = Mix_LoadWAV( "button.wav" );
    if( gbutton == NULL )
    {
        printf( "Failed to load button sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    //Load sheet image of main button
    //If fail to do so state, else move to splitting of sheet image
    if( !gButtonSpriteSheetTexture.loadFromFile( "button.png" ) )
    {
        printf( "Failed to load button sprite texture!\n" );
        success = false;
    }
    else
    {
        //This sections splits the sheet image of the button into 2 different textures
        //There are 2 buttons one darker for when scrolling over and one lighter when not scrolling over
        //This splits the single image into 2 button textures
        for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
        {
            //top left corner
            gSpriteClips[ i ].x = 0;
            //buttons are 157 length
            gSpriteClips[ i ].y = i * 157;
            //200
            gSpriteClips[ i ].w = BUTTON_WIDTH;
            //100
            gSpriteClips[ i ].h = BUTTON_HEIGHT;
        }
        
        //Set button position
        gButtons[ 0 ].setPosition( 343, 400 );
     
    }
    */
    return success;
}

void closeMedia()
{
    //Free loaded images
    blueshroomTex.free();
    background.free();
    gButtonSpriteSheetTexture.free();
    
    //Destroy window
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    //Mix_Quit();
}

float distanceSquared (int x1, int y1, int x2, int y2)
{
    int difX = x2-x1;
    int difY = y2-y1;
    return difX*difX + difY*difY;
}

bool checkCollision ( Circle& a, Circle& b)
{
    //Calculate total readius squared
    int totalRadiusSquared;
    totalRadiusSquared = (a.r + b.r)*(a.r + b.r);
    
    //if distance between the circles is less than the sum of their radii collision occurs
    if( distanceSquared(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
    {
        return true;
    }
    
    return false;
}

bool checkCollision( Circle& a, SDL_Rect& b )
{
    //Closest point on collision box
    int cX, cY;
    
    //Find closest x offset
    if( a.x < b.x )
    {
        cX = b.x;
    }
    else if( a.x > b.x + b.w )
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }
    
    //Find closest y offset
    if( a.y < b.y )
    {
        cY = b.y;
    }
    else if( a.y > b.y + b.h )
    {
        cY = b.y + b.h;
    }
    else
    {
        cY = a.y;
    }
    
    //
    //If the closest point is inside the circle
    if( distanceSquared( a.x, a.y, cX, cY ) < (a.r * a.r) )
    {
        //This box and the circle have collided
        return true;
    }

    //If the shapes have not collided
    return false;
}

int main()
{
    //Start up SDL and create window
    cout<<7*cos(-atan(1/-1))<<endl;
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    //Load media
    if( !loadMedia() )
    {
        printf( "Failed to load media!\n" );
    }
    //Main loop flag
    bool quit = false;
    
    //Set the wall
    SDL_Rect wall1,wall2,wall3,wall4;
    wall1.x = 300;
    wall1.y = 150;
    wall1.w = 100;
    wall1.h = 100;
    
    wall2.x = 700;
    wall2.y = 150;
    wall2.w = 100;
    wall2.h = 100;
    
    wall3.x = 300;
    wall3.y = 450;
    wall3.w = 100;
    wall3.h = 100;
    
    wall4.x = 700;
    wall4.y = 450;
    wall4.w = 100;
    wall4.h = 100;
    
    //Event handler
    SDL_Event e;
    mushroomArmy blueshrooms;
    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            blueshrooms.handleEvent(e);
        }
        if ((e.key.keysym.sym == SDLK_RIGHT) or (e.key.keysym.sym == SDLK_LEFT) or (e.key.keysym.sym == SDLK_UP) or (e.key.keysym.sym == SDLK_DOWN))
        {
            blueshrooms.move(wall1);
        }

        //Clear screen
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer );
        
        //Render background texture to screen
        //background.render( 0, 0 );
        
        //Render wall
        SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderDrawRect( renderer, &wall1);
        /*
        SDL_RenderDrawRect( renderer, &wall2);
        SDL_RenderDrawRect( renderer, &wall3);
        SDL_RenderDrawRect( renderer, &wall4);
        */
        
        //Render image to the screen
        blueshrooms.renderArmy();
        
        //Update screen
        SDL_RenderPresent( renderer );
        
        SDL_Delay(15);
    }
    
    //Free resources and close SDL
    closeMedia();
    
    return 0;
}


