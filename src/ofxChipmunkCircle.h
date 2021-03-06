#ifndef OFXCHIPMUNK_CIRCLE_H
#define OFXCHIPMUNK_CIRCLE_H

#include "ofxChipmunkBody.h"
#include "ofxChipmunkShape.h"

namespace ofxChipmunk {

class Circle: public ofxChipmunk::DynamicBody, public ofxChipmunk::ShapeCircle{
public:
    Circle();
	Circle(cpSpace* space, float radius, float mass=1);
    Circle(ShapeCircle* src, float mass=1);

	void setup(cpSpace* space, float radius, float mass=1);
};

} // namespace ofxChipmunk

#endif // OFXCHIPMUNK_CIRCLE_H
