#include "ofxChipmunkBody.h"
#include "ofxChipmunkConstraint.h"

namespace ofxChipmunk {

static std::vector<cpConstraint*> constraintsToRemove;

static void constraintRemoveQuery(cpBody* body, cpConstraint *constraint, void *data){
	if(!constraint)
		return;

	constraintsToRemove.push_back(constraint);
}

Body::Body():body(nullptr){

}

Body::~Body(){
	if(body){
		//clean up all constraints
		constraintsToRemove.clear();
		cpBodyEachConstraint(body, &constraintRemoveQuery, (void*)this);
		for(auto c: constraintsToRemove){
			((Constraint*)cpConstraintGetUserData(c))->constraint = nullptr;
			cpSpaceRemoveConstraint(cpConstraintGetSpace(c), c);
			cpConstraintDestroy(c);
		}

		//remove body
		cpSpaceRemoveBody(cpBodyGetSpace(body), body);
		cpBodyFree(body);
		body = nullptr;
	}
}

void Body::setup(cpSpace *space, cpBody *b){
	body = cpSpaceAddBody(space, b);
	cpBodySetUserData(body, this);
}

glm::vec2 ofxChipmunk::Body::getPosition(){
	return toOf(cpBodyGetPosition(body));
}

void Body::setPosition(glm::vec2 pos){
	cpBodySetPosition(body, toChipmunk(pos));
}

void Body::move(glm::vec2 m){
	setPosition(getPosition()+m);
}

float Body::getRotation(){
	return cpBodyGetAngle(body);
}

void Body::setRotation(float radians){
	cpBodySetAngle(body, radians);
}

bool Body::isSleeping(){
	return cpBodyIsSleeping(body);
}

void Body::addForce(glm::vec2 force, glm::vec2 offset){
	cpBodyApplyForceAtLocalPoint(body, toChipmunk(force), toChipmunk(offset));
}

////////////////////////////////////////////////////
DynamicBody::DynamicBody(){

}

DynamicBody::~DynamicBody(){

}

void DynamicBody::setup(cpSpace* space,float mass, cpFloat moment){
	Body::setup(space, cpBodyNew(mass, moment));
}

/////////////////////////////////////////////////////////////

StaticBody::StaticBody(){

}

StaticBody::StaticBody(cpSpace *space){
	setup(space);
}

void ofxChipmunk::StaticBody::setup(cpSpace *space){
	Body::setup(space, cpBodyNewStatic());
}

////////////////////////////////////////////////////////
KinematicBody::KinematicBody(){
}

KinematicBody::KinematicBody(cpSpace *space){
	setup(space);
}

void KinematicBody::setup(cpSpace *space){
	Body::setup(space, cpBodyNewKinematic());
}


}// namespace ofxChimpunk
