/* Copyright 2017 Dmitry Mordvinov, Gleb Zakharov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */
#include "box2DRobot.h"

#include <Box2D/Box2D.h>
#include <QDebug>

#include "src/engine/model/physics/box2DPhysicsEngine.h"
#include "twoDModel/engine/model/robotModel.h"
#include "twoDModel/engine/model/constants.h"
#include "box2DWheel.h"

using namespace twoDModel::model::physics;
using namespace parts;

box2DRobot::box2DRobot(box2DPhysicsEngine *engine, twoDModel::model::RobotModel * const robotModel
		, b2Vec2 pos, float angle)
	: model(robotModel), engine(engine), world(engine->box2DWorld())
{
	float32 widthRobotM =  engine->pxToM(model->info().size().width());
	float32 heightRobotM = engine->pxToM(model->info().size().height());

	b2BodyDef bodyDef;
	bodyDef.position = pos;
	bodyDef.angle = angle;
	bodyDef.type = b2_dynamicBody;
	body = world.CreateBody(&bodyDef);

	b2FixtureDef robotFixture;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox( widthRobotM / 2, heightRobotM / 2 );
	robotFixture.shape = &polygonShape;
	robotFixture.density = 0.01f;
	robotFixture.friction = model->info().friction();
	body->CreateFixture(&robotFixture);
	body->SetUserData( this );

	connectWheels();
}

box2DRobot::~box2DRobot() {
	for (auto i = joints.begin(); i != joints.end(); i++)
		world.DestroyJoint(*i);
	for (auto i = wheels.begin(); i != wheels.end(); i++)
		delete *i;
	world.DestroyBody(body);
}

void box2DRobot::Stop()
{
	if (isStopping){
		body->SetLinearVelocity(b2Vec2(0, 0));
		body->SetAngularVelocity(0);
	}
}

void box2DRobot::StartStopping()
{
	isStopping = true;
}

void box2DRobot::FinishStopping()
{
	isStopping = false;
}

bool box2DRobot::IsStopping()
{
	return isStopping;
}

void box2DRobot::connectWheels() {
	qreal wheelWidthShift = 5;
	QPointF leftUpCorner = QPointF(-model->info().size().width() / 2, -model->info().size().height() / 2);

	QPointF posLeftWheelFromRobot = QPointF(twoDModel::robotWheelDiameterInPx / 2, wheelWidthShift);
	QPointF posRightWheelFromRobot = QPointF(twoDModel::robotWheelDiameterInPx / 2
			, model->info().size().width() - wheelWidthShift);

	b2Vec2 posLeftWheel = body->GetWorldPoint(engine->positionToBox2D(posLeftWheelFromRobot + leftUpCorner));
	b2Vec2 posRightWheel = body->GetWorldPoint(engine->positionToBox2D(posRightWheelFromRobot + leftUpCorner));

	box2DWheel *leftWheel = new box2DWheel(posLeftWheel, body->GetAngle(), *this);
	box2DWheel *rightWheel = new box2DWheel(posRightWheel, body->GetAngle(), *this);
	wheels.push_back(leftWheel);
	wheels.push_back(rightWheel);
	connectWheel(*leftWheel);
	connectWheel(*rightWheel);
}

void box2DRobot::connectWheel(box2DWheel &wheel){
	b2RevoluteJointDef revDef;
	revDef.bodyA = wheel.body;
	revDef.bodyB = body;
	revDef.collideConnected = false;

	revDef.localAnchorA = wheel.body->GetLocalCenter();
	revDef.localAnchorB = body->GetLocalPoint(wheel.body->GetWorldCenter());

	revDef.referenceAngle = 0;
	revDef.enableLimit = true;
	revDef.lowerAngle = 0;
	revDef.upperAngle = 0;

	b2Joint *joint = world.CreateJoint(&revDef);
	joints.push_back(joint);
}