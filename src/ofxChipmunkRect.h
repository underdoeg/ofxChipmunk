#ifndef OFXCHIPMUNK_QUAD_H
#define OFXCHIPMUNK_QUAD_H

#include "ofxChipmunkShape.h"
#include "ofxChipmunkBody.h"

namespace ofxChipmunk {

class Rect: public ofxChipmunk::DynamicBody, public ofxChipmunk::ShapeRect{
public:
    Rect();
	Rect(cpSpace* space, ofRectangle bounds, float mass=1);

	void setup(cpSpace* space, ofRectangle bounds, float mass=1);
};

} // namespace ofxChipmunk

#endif // OFXCHIPMUNK_CIRCLE_H
