#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"

#define MAX_FRAMES 100
typedef unsigned int uint;
class Animation
{
private:
	SDL_Rect frames[MAX_FRAMES];
public:
	//Original variables----------------------------------
	bool loop = true;
	bool pingpong = false;
	float speed = 1.0f;
	
	float current_frame = 0;
	int numFrames = 0;
	int loops = 0;
	enum pingpong
	{
		forward,
		backward
	} direction = forward;

	//Added variables-------------------------------------

	bool invert = false;
	bool first_loop = true;
	bool finished = false;
	bool autoReset = false;

public:
	Animation()
	{}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), numFrames(anim.numFrames)
	{
		SDL_memcpy(&frames, anim.frames, sizeof(frames));
	}
	void ResizeFrame(int frameNum, int position_x, int position_y, int measure_x, int measure_y)
	{
		frames[frameNum].x = (int)position_x;
		frames[frameNum].y = (int)position_y;
		frames[frameNum].w = (int)measure_x;
		frames[frameNum].h = (int)measure_y;
	}

	void PushBack(const SDL_Rect& rect)
	{
		frames[numFrames++] = rect;
	}

	SDL_Rect&  ReturnFrame(int i)
	{
		return frames[i];
	}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		switch (direction)
		{
		case pingpong::forward:
		{
			current_frame += speed*dt;
			if (current_frame >= numFrames)
			{
				current_frame = (loop || pingpong) ? 0.0f : numFrames - 1;
				direction = pingpong ? pingpong::backward : pingpong::forward;
				loops++;
			}
		}
		break;
		case pingpong::backward:
		{
			current_frame -= speed*dt;
			if (current_frame <= 0.0f)
			{
				current_frame = 0.0f;
				direction = pingpong::forward;
				loops++;
			}
		}
		break;
		}

		return frames[(int)current_frame];
	}
	float GetCurrentFrameNum()
	{
		return current_frame;
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		finished = false;
		current_frame = 0;
		loops = 0;

	}

	//Added methods--------------------------------------------------------------------------------

	SDL_Rect& GetFrame()
	{
		return frames[(int)current_frame];
	}

	//Description: If animation finish switch finished to true (Used in module continue fire animation)
	SDL_Rect& GetFrameEx()
	{
		current_frame += speed;
		if (current_frame >= numFrames) {

			finished = true;
			return frames[numFrames - 1];
		}
		return frames[(int)current_frame];
	}
	//Description: Increment a number of frames to current frame (Implemented in street lights)
	SDL_Rect& AddFrame(uint added_frames)
	{
		uint i = 0;
		float frame = current_frame;
		while (i < added_frames) {
			++frame;
			if (frame >= numFrames)
				frame = 0;
			++i;
		}
		return frames[(int)frame];
	}
	//Description: This runs an animation from 0 to its maximum and from its maximum to 0 infinitely (Implemented in orange laser)
	SDL_Rect& LoopAnimation()
	{
		if (first_loop) {
			first_loop = false;
			current_frame += speed;
			return frames[0];
		}
		else {
			if (invert == false) {

				current_frame += speed;

				if (current_frame >= numFrames) {
					current_frame = numFrames - speed;
					invert = true;
				}
			}
			else {

				current_frame -= speed;

				if (current_frame < 0) {
					current_frame = speed;
					invert = false;
				}
			}
			return frames[(int)current_frame];
		}
	}

};

#endif
