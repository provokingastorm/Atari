// Copy the files "particles.png" and "menu.wav"
// from the folder "precompiled" to the folder with
// executable file. Also copy hge.dll and bass.dll
// to the same folder.


#include "..\..\include\hge.h"
#include "..\..\include\hgeanim.h"

HGE *hge=0;

// Quad is the basic primitive in HGE
// used for rendering graphics.
// Quad contains 4 vertices, numbered
// 0 to 3 clockwise.
hgeAnimation* sprite;

// Handle for a sound effect
HEFFECT snd;

// Some "gameplay" variables and constants
float x=100.0f, y=100.0f;
float dx=0.0f, dy=0.0f;
float cooldown=0.0f;

const float speed=90;
const float friction=0.98f;
const float cooldownTimer=1.0f;
const float screen_width=1280.0f;
const float screen_height=720.0f;
const float sprite_width = 64.0f;
const float sprite_height = 64.0f;
const float sprite_half_width = sprite_width * 0.5f;
const float sprite_half_height = sprite_height * 0.5f;
const float max_collision_horizontal = (screen_width-sprite_half_width);
const float max_collision_vertical = (screen_height-sprite_half_height);

void onCollision()
{
	cooldown=cooldownTimer;
	hge->Effect_Play(snd);
}

bool FrameFunc()
{
	// Get the time elapsed since last call of FrameFunc().
	// This will help us to synchronize on different
	// machines and video modes.
	float dt=hge->Timer_GetDelta();

	// Process keys
	if(cooldown <= 0.0f)
	{
		if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
		if (hge->Input_GetKeyState(HGEK_LEFT)) dx-=speed*dt;
		if (hge->Input_GetKeyState(HGEK_RIGHT)) dx+=speed*dt;
		if (hge->Input_GetKeyState(HGEK_UP)) dy-=speed*dt;
		if (hge->Input_GetKeyState(HGEK_DOWN)) dy+=speed*dt;
	}
	else
	{
		cooldown -= dt;
	}

	// Do some movement calculations and collision detection
	dx*=friction; dy*=friction; x+=dx; y+=dy;
	if(x > max_collision_horizontal)
	{
		x = max_collision_horizontal - (x - max_collision_horizontal);
		dx = -dx;
		onCollision();
	}

	if(x < sprite_half_width)
	{
		x = sprite_width-x;
		dx = -dx;
		onCollision();
	}

	if(y > max_collision_vertical)
	{
		y = max_collision_vertical - (y-max_collision_vertical);
		dy = -dy;
		onCollision();
	}

	if(y < sprite_half_height)
	{
		y = sprite_height - y;
		dy = -dy;
		onCollision();
	}

	sprite->Update(dt);

	// Continue execution
	return false;
}

// This function will be called by HGE when
// the application window should be redrawn.
// Put your rendering code here.
bool RenderFunc()
{
	// Begin rendering quads.
	// This function must be called
	// before any actual rendering.
	hge->Gfx_BeginScene();

	// Clear screen with black color
	hge->Gfx_Clear(0);

	// Render quads here. This time just
	// one of them will serve our needs.
	sprite->Render(x, y);

	// End rendering and update the screen
	hge->Gfx_EndScene();

	// RenderFunc should always return false
	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Get HGE interface
	hge = hgeCreate(HGE_VERSION);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "atari.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "Atari Game");

	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, static_cast<int>(screen_width));
	hge->System_SetState(HGE_SCREENHEIGHT, static_cast<int>(screen_height));
	hge->System_SetState(HGE_SCREENBPP, 32);

	if(hge->System_Initiate())
	{
		// Load sound and texture
		snd=hge->Effect_Load("Laser_Shoot.wav");
		HTEXTURE tex=hge->Texture_Load("Sprite-0002.png");
		if(!snd || !tex)
		{
			// If one of the data files is not found, display
			// an error message and shutdown.
			MessageBox(NULL, "Can't load MENU.WAV or Sprite-0002.PNG", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}

		sprite=new hgeAnimation(tex, 3, 30.0f, 0.0f, 0.0f, sprite_width, sprite_height);
		sprite->SetMode(HGEANIM_FWD|HGEANIM_LOOP|HGEANIM_PINGPONG);
		sprite->Play();

		// Let's rock now!
		hge->System_Start();

		delete sprite;

		// Free loaded texture and sound
		hge->Texture_Free(tex);
		hge->Effect_Free(snd);
	}
	else MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
