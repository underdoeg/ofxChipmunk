#include <chrono>

#include "ofxChipmunkWorld.h"

extern "C"{
#include "cpHastySpace.h"
}

namespace ofxChipmunk {


World::World(){
    //space = cpHastySpaceNew();
    //cpHastySpaceSetThreads(space, 4);
    space = cpSpaceNew();
    cpSpaceSetUserData(space, this);

    setGravity();
    bLowFPS = false;

	ofAddListener(ofEvents().mousePressed, this, &World::onMouseDown);
	ofAddListener(ofEvents().mouseDragged, this, &World::onMouseDrag);
	ofAddListener(ofEvents().mouseReleased, this, &World::onMouseUp);
}

World::~World(){
    //threadStop();
    //thread.join();
	//cpSpaceFree(space);
}

/*
void World::runThreaded(const int runsPerUpdate){
    bKeepThreadRunning = true;

    thread = std::thread([=](){
        std::chrono::time_point<std::chrono::system_clock> start, end;
        while(true){

            threadLock();
            if(!bKeepThreadRunning){
                ofLog() << "STOP THREAD";
                break;
            }
            for(int i=0;i<runsPerUpdate; i++){
                update(1./double(ofGetTargetFrameRate()*runsPerUpdate));
            }
            threadUnlock();

        }
    });
}

void World::threadLock(){
    mutex.lock();
}

void World::threadUnlock(){
    mutex.unlock();
}

void World::threadStop(){
    threadLock();
    bKeepThreadRunning = false;
    threadUnlock();
}

void World::waitForThread(){
    threadLock();

    threadUnlock();
}

void World::continueThread(){
    threadUnlock();
    threadUnlock();
}
*/

void World::update(){
    if(ofGetFrameNum() < 10)
        return;

    int targetFPS = ofGetTargetFrameRate();
    if(targetFPS == 0){
        ofLogWarning("ofxChipmunk") << "no fixed framerate set, it is recommended to use ofSetFrameRate()";
        targetFPS = 60;
    }

    int curFPS = ofGetFrameRate();
    if(curFPS < targetFPS*.95 && ofGetFrameNum() > 60){
        if(!bLowFPS)
            ofLogWarning("ofxChipmunk") << "current framerate (" << curFPS << ") is bellow target framerate (" << targetFPS << "), physics may behave strangely";
        bLowFPS = true;
    }else{
        bLowFPS = false;
    }

    //cpHastySpaceStep(space, 1.f/float(targetFPS));
    update(1./double(targetFPS));
}

void World::update(double timeStep){
    cpSpaceStep(space, timeStep);
}

void World::draw(){
    drawSpace(space);
}

void World::setGravity(glm::vec2 g){
	cpSpaceSetGravity(space, toChipmunk(g));
}

glm::vec2 World::getGravity(){
	return toOf(cpSpaceGetGravity(space));
}

void World::createFloor(){
    floor = createStaticLine(glm::vec2(0, ofGetHeight()), glm::vec2(ofGetWidth(), ofGetHeight()), 2);
}

void World::createWallLeft(){
    wallLeft = createStaticLine(glm::vec2(0, 0), glm::vec2(0, ofGetHeight()), 2);
}

void World::createWallRight(){
    wallRight = createStaticLine(glm::vec2(ofGetWidth(), 0), glm::vec2(ofGetWidth(), ofGetHeight()), 2);
}

void World::createBounds(){
    createFloor();
    createWallLeft();
    createWallRight();
}

void World::setPicking(bool state){
    bPickingEnabled = state;
}

Body* World::getNearestBody(glm::vec2 pos, float radius){
    cpShape* shape = cpSpacePointQueryNearest(space, toChipmunk(pos), radius, CP_SHAPE_FILTER_ALL, NULL);
    if(!shape)
        return nullptr;
	return (Body*)cpBodyGetUserData(cpShapeGetBody(shape));
}

void World::setNumIterations(int i){
	cpSpaceSetIterations(space, i);
}

int World::getNumIterations(){
	return cpSpaceGetIterations(space);
}

shared_ptr<Circle> World::createCircle(float radius, float mass){
    return shared_ptr<Circle>(new Circle(space, radius, mass));
}

shared_ptr<Rect> World::createRect(ofRectangle rect, float mass){
    return shared_ptr<Rect>(new Rect(space, rect, mass));
}

shared_ptr<Line> World::createLine(glm::vec2 a, glm::vec2 b, float radius, float mass){
    return shared_ptr<Line>(new Line(space, a, b, radius, mass));
}

shared_ptr<Polygon> World::createPoly(std::vector<glm::vec2>& points, float radius, float mass){
	return shared_ptr<Polygon>(new Polygon(space, points, radius, mass));
}

shared_ptr<Polygon> World::createPoly(ofPolyline poly, float radius, float mass){
	return shared_ptr<Polygon>(new Polygon(space, poly, radius, mass));
}

shared_ptr<Composite> World::createComposite(Composite::Definition &definition){
    return shared_ptr<Composite>(new Composite(space, definition));
}

shared_ptr<KinematicBody> World::createKinematicBody(glm::vec2 position){
    KinematicBody* body = new KinematicBody(space);
    body->setPosition(position);
    return shared_ptr<KinematicBody>(body);
}

shared_ptr<StaticBody> World::createStaticBody(glm::vec2 position){
    StaticBody* body = new StaticBody(space);
    body->setPosition(position);
    return shared_ptr<StaticBody>(body);
}

shared_ptr<StaticLine> World::createStaticLine(glm::vec2 a, glm::vec2 b, float radius){
    return shared_ptr<StaticLine>(new StaticLine(space, a, b, radius));
}

shared_ptr<StaticRect> World::createStaticRect(ofRectangle rect){
    return shared_ptr<StaticRect>(new StaticRect(space, rect));
}

shared_ptr<StaticCircle> World::createStaticCircle(float radius){
    return shared_ptr<StaticCircle>(new StaticCircle(space, radius));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SPRINGS


shared_ptr<Spring> World::createSpring(shared_ptr<Body> a, shared_ptr<Body> b, float stiffness, float damping){
	return createSpring(a, b, glm::vec2(0, 0), glm::vec2(0, 0), glm::distance(a->getPosition(),b->getPosition()), stiffness, damping);
}

shared_ptr<Spring> World::createSpring(Body *a, Body *b, float stiffness, float damping){
	return createSpring(a, b, glm::vec2(0, 0), glm::vec2(0, 0), glm::distance(a->getPosition(), b->getPosition()), stiffness, damping);
}

shared_ptr<Spring> World::createSpring(shared_ptr<Body> a, shared_ptr<Body> b, glm::vec2 anchorA, glm::vec2 anchorB, float distance, float stiffness, float damping){
    return createSpring(a.get(), b.get(), anchorA, anchorB, distance, stiffness, damping);
}

shared_ptr<Spring> World::createSpring(Body *a, Body *b, glm::vec2 anchorA, glm::vec2 anchorB, float distance, float stiffness, float damping){
    return shared_ptr<Spring>(new Spring(space, a, b, anchorA, anchorB, distance, stiffness, damping));
}

shared_ptr<PivotJoint> World::createPivotJoint(shared_ptr<Body> a, shared_ptr<Body> b, glm::vec2 anchorA, glm::vec2 anchorB){
    return createPivotJoint(a.get(), b.get(), anchorA, anchorB);
}

shared_ptr<PivotJoint> World::createPivotJoint(Body *a, Body *b, glm::vec2 anchorA, glm::vec2 anchorB){
	return shared_ptr<PivotJoint>(new PivotJoint(space, a, b, anchorA, anchorB));
}

shared_ptr<SimpleMotor> World::createSimpleMotor(shared_ptr<Body> a, shared_ptr<Body> b, float rate){
	return createSimpleMotor(a.get(), b.get(), rate);
}

shared_ptr<SimpleMotor> World::createSimpleMotor(Body *a, Body *b, float rate){
	return shared_ptr<SimpleMotor>(new SimpleMotor(space, a, b, rate));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<DynamicBody> World::createBodyForShape(Shape *shape, float mass){
    shared_ptr<DynamicBody> ret;
    switch(shape->getType()){
    case Shape::Type::Circle:
        ret = std::dynamic_pointer_cast<DynamicBody>(shared_ptr<Circle>(new Circle((ShapeCircle*)shape, mass)));
        break;
    case Shape::Type::Line:
        ret = std::dynamic_pointer_cast<DynamicBody>(shared_ptr<ShapeLine>(new Line((ShapeLine*)shape, mass)));
        break;
    case Shape::Type::Polygon:
        ret = std::dynamic_pointer_cast<DynamicBody>(shared_ptr<ShapePolygon>(new Polygon((ShapePolygon*)shape, mass)));
        break;
    case Shape::Type::Rectangle:
        //TODO
        ofLogWarning("ofxChipmunk") << "createBodyForShape not implemented for rectangles";
        break;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void World::onMouseDown(ofMouseEventArgs &args){
    if(!bPickingEnabled) return;
    Body* body = getNearestBody(args);
    if(!body)
        return;

    if(!mouseBody)
        mouseBody = createKinematicBody(args);

    mouseJoint = createPivotJoint(mouseBody.get(), body);
    mouseJoint->setMaxForce(500000.0);
    mouseJoint->setErrorBias(cpfpow(1.0f - 0.15f, ofGetFrameRate()==0?60.f:ofGetFrameRate()));
}

void World::onMouseDrag(ofMouseEventArgs &args){
    if(!bPickingEnabled) return;

    if(mouseJoint)
        mouseBody->setPosition(args);
}

void World::onMouseUp(ofMouseEventArgs &args){
    if(!bPickingEnabled) return;

    if(mouseJoint)
        mouseJoint.reset();
}


} // namespace ofxChipmunk
