#include "ofxChipmunkShape.h"
#include "ofxChipmunkUtils.h"

namespace ofxChipmunk {

Shape::Shape():shape(nullptr){

}

Shape::~Shape(){
	cpShapeFree(shape);
}

void Shape::setup(cpSpace *space, cpShape *s){
	shape = cpSpaceAddShape(space, s);
	setFriction(OFXCHIPMUNK_DEFAULT_FRICTION);
	setElasticity(OFXCHIPMUNK_DEFAULT_ELASTICITY);
}

void Shape::setElasticity(float value){
    cpShapeSetElasticity(shape, value);
}

void Shape::setFriction(float friction){
    cpShapeSetFriction(shape, friction);
}

//
ShapeCircle::ShapeCircle(){

}

ShapeCircle::ShapeCircle(cpSpace *space, cpBody *body, float radius, ofVec2f offset){
	setup(space, body, radius, offset);
}

void ShapeCircle::setup(cpSpace *space, cpBody* body, float radius, ofVec2f offset){
	Shape::setup(space, cpCircleShapeNew(body, radius, toChipmunk(offset)));
}

void ShapeCircle::setRadius(float r){
    cpCircleShapeSetRadius(shape, r);
}

float ShapeCircle::getRadius(){
    return cpCircleShapeGetRadius(shape);
}

//
ShapeRect::ShapeRect(){

}

ShapeRect::ShapeRect(cpSpace *space, cpBody *body, ofRectangle bounds){
	setup(space, body, bounds);
}

void ShapeRect::setup(cpSpace *space, cpBody *body, ofRectangle bounds){
    Shape::setup(space, cpBoxShapeNew(body, bounds.width, bounds.height, 0));
}

//
ShapePolygon::ShapePolygon(){}

ShapePolygon::ShapePolygon(cpSpace *space, cpBody *body, ofPolyline poly){
	setup(space, body, poly);
}

ShapePolygon::ShapePolygon(cpSpace *space, cpBody *body, std::vector<ofVec2f> &points){
	setup(space, body, points);
}

void ShapePolygon::setup(cpSpace *space, cpBody *body, ofPolyline poly){
    std::vector<ofVec2f> vecs;
    for(auto& p: poly){
        vecs.push_back(p);
    }
    setup(space, body, vecs);
}

void ShapePolygon::setup(cpSpace *space, cpBody *body, std::vector<ofVec2f> &points){
    cpVect verts[points.size()];
    //memcpy(verts, points.data(), sizeof(cpVect)*points.size());
    for(size_t i=0; i<points.size(); i++){
        verts[i] = toChipmunk(points[i]);
    }

    setup(space, body, points.size(), verts);
}

void ShapePolygon::setup(cpSpace *space, cpBody *body, int nPoints, cpVect *verts){
    Shape::setup(space, cpPolyShapeNew(body, nPoints, verts, cpTransformIdentity, 0.0));
}

//

} // namespace ofxChimpunk
