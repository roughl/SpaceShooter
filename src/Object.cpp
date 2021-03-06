#include <string>
#include <iostream>
#include <math.h>
#include <complex>

#include "SDL_rotozoom.h"

#include "Object.hpp"
#include "Engine.hpp"
#include "global.hpp"

std::list<Object*> Object::AllObjects;

Object::Object(Engine* mengine, ObjectType type, double x, double y, double speedx, double speedy)
:Type(type), Position(x, y), Speed(speedx + speedy * 1i), Angle(0),
    mEngine(mengine), Image(0), RotatedImage(NULL), Radius(20)
{
	AllObjects.push_back(this);
}

int Object::getMiddleX()
{   //0.5f has been outcommented, because its not much
    return (int)(Position.real() - (getRotatedImg()->w / 2) );//+ 0.5l);
} //todo: very slow to check the size of the rotatedimg each time?

int Object::getMiddleY()
{   //0.5f has been outcommented, because its not much
    return (int)(Position.imag() - (getRotatedImg()->h / 2) );//+ 0.5l);
} //todo: very slow to check the size of the rotatedimg each time?

bool Object::Update()
{
	positionUpdate();
	return 1;	
}

void Object::positionUpdate()
{
    //first, calculate the new position
    double real = Position.real()+Speed.real();
    double imag = Position.imag()+Speed.imag();

    //then, wrap the position around if its in the wrong place
    //if the object is too fast (speed bigger than screenwidth) it should disappear from the screen, but such a speed...
    if(Position.real() < 0)
        real += (double)SCREEN_WIDTH;
    if(Position.imag() < 0)
        imag += (double)SCREEN_HEIGHT;
    if(Position.real() > SCREEN_WIDTH)
        real -= (double)SCREEN_WIDTH;
    if(Position.imag() > SCREEN_HEIGHT)
        imag -= (double)SCREEN_HEIGHT;

    Position = std::complex<double>(real, imag);
}

void Object::rotate(double angle)
{
    Angle += angle;
}

void Object::incSpeed(double inc)
{
    std::complex<double> SpeedIncAngle(0,((Angle+90)/180*M_PI));
    Speed += inc * exp(-SpeedIncAngle);
}

SDL_Surface* Object::getRotatedImg()
{
    if(Image != NULL)
    {
        SDL_FreeSurface(RotatedImage); //Free the Memory of the old RotatedImage
        //RotatedImage = NULL; //unnecessary
        RotatedImage = rotozoomSurface(Image, Angle, 0.9, 1);//Allocate new Memory for the new RotatedImage
    }
    if(Image == NULL)
        std::cout << "Error: Image == NULL" << std::endl; //should absolutely never happen
    return RotatedImage;
}

bool Object::getCriticalPosition()
{   //todo: calculate the critical position in the derivated objects, such as asteroid.cpp. overwrite this function!
    return false;
}

Object::~Object()
{
  SDL_FreeSurface(RotatedImage);
}

void Object::Show()
{
	if(!Image)  Image = mEngine->getObjectImage(Type);
  SDL_Surface* tmp = mEngine->getScreen();
  if(tmp == 0)
      std::cout << "tmp is 0, cant blit that" << std::endl;
  SDL_Rect offset;
  offset.x = getMiddleX();
  offset.y = getMiddleY();
  SDL_BlitSurface(getRotatedImg(), NULL, tmp, &offset);
}

void Object::UpdateAll()
{
  std::list<Object*>::iterator ObjItr;
  for(ObjItr = AllObjects.begin(); ObjItr != AllObjects.end(); ObjItr++)
  {
  	if(ObjItr != AllObjects.end())
  	{
	  	bool isLive = (*ObjItr)->Update();
/*	  	if(!isLive)
	  	{
	  		delete (*ObjItr);
	  		_AllObjects.remove(*ObjItr);
	  		ObjItr--;
	  	}*/
  	}
  }
}

void Object::ShowAll()
{
  std::list<Object*>::iterator ObjItr;
  for(ObjItr = AllObjects.begin(); ObjItr != AllObjects.end(); ObjItr++)
  {

	  (*ObjItr)->Show();
  }	
}
