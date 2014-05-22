/*
 * NotifierTest.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Dawid Drozd
 */

#include "gtest/gtest.h"
#include "component/Notifier.h"

using namespace KoalaComponent;

MAKE_NOTIFICATION( Test1 );
MAKE_NOTIFICATION( Test2 );
MAKE_NOTIFICATION( Test3, int );
MAKE_NOTIFICATION( Test4, float );

//We use short notification names for better performance normally we have more.
#define OLD_NOTIFICATION_1 "a"
#define OLD_NOTIFICATION_2 "b"
#define OLD_NOTIFICATION_3 "c"
#define OLD_NOTIFICATION_4 "d"

struct TestClazz : CCObject
{
	TestClazz(){}

	int deliveredCount = 0;

	void onOldNotification( CCObject* pObject )
	{
		++deliveredCount;
	}

	void onOldNotificationFloat( CCObject* pObject )
	{
		//We use static cast for better performance
		CCFloat* pFloat = static_cast<CCFloat*>( pObject );
		deliveredCount += pFloat->getValue();
	}

	void onOldNotificationInt( CCObject* pObject )
	{
		//We use static cast for better performance
		CCInteger* pInteger = static_cast<CCInteger*>( pObject );
		deliveredCount += pInteger->getValue();
	}

	void onDeliverNotification()
	{
		++deliveredCount;
	}

	void onDeliverNotification2( float value )
	{
		deliveredCount += value;
	}

	void onDeliverNotification3( int value )
	{
		deliveredCount += value;
	}
};

TEST( Notifier, TestOfNotification )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ( 0, testClazz.deliveredCount );

	notifier.notify( getNotificationTest1() );

	EXPECT_EQ( 1, testClazz.deliveredCount );

	notifier.notify( getNotificationTest1() );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 3, testClazz.deliveredCount );

	notifier.removeAllForObject( &testClazz );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 3, testClazz.deliveredCount );

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	EXPECT_EQ( 3, testClazz.deliveredCount );

	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 4, testClazz.deliveredCount );

	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 4, testClazz.deliveredCount );


	//////////////////////////////////////////////////////////////////////////////////////

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification( getNotificationTest2(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	//Here listeners are not still added because before notifications we apply changes
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );

	notifier.notify( getNotificationTest1() );

	//Changes should be applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	EXPECT_EQ( 5, testClazz.deliveredCount );

	notifier.notify( getNotificationTest2() );
	EXPECT_EQ( 6, testClazz.deliveredCount );

	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 6, testClazz.deliveredCount );

	notifier.notify( getNotificationTest2() );
	EXPECT_EQ( 7, testClazz.deliveredCount );

	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.notify( getNotificationTest1() );

	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	EXPECT_EQ( 8, testClazz.deliveredCount );

	notifier.removeAllForObject( &testClazz );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 8, testClazz.deliveredCount );

	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest2().tag ) );
}

TEST( Notifier, TestOfNotificationChangesStack )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification( getNotificationTest2(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest2().tag ) );

	EXPECT_EQ( 0, testClazz.deliveredCount );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 1, testClazz.deliveredCount );

	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.removeAllForObject( &testClazz );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.removeAllForObject( &testClazz );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.notify( getNotificationTest3(), 0x01 );

	//changes should be applied
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest2().tag ) );
}

TEST( Notifier, TestOfNotificationChangesStackDoubleCommands )
{
	Notifier notifier;
	TestClazz testClazz;

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification( getNotificationTest2(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest2().tag ) );

	EXPECT_EQ( 0, testClazz.deliveredCount );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 1, testClazz.deliveredCount );

	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.removeAllForObject( &testClazz );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.removeAllForObject( &testClazz );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	notifier.addNotification( getNotificationTest2(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );

	//Changes still not applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	notifier.notify( getNotificationTest3(), 0x01 );

	//changes should be applied
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );

	EXPECT_EQ( 1, testClazz.deliveredCount );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 2, testClazz.deliveredCount );

	notifier.addNotification( getNotificationTest1(),
							  Utils::makeCallback( &testClazz, &TestClazz::onDeliverNotification ) );
	//Now we have double notification
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 4, testClazz.deliveredCount );
	EXPECT_EQ( 2, notifier.getListenersCount( getNotificationTest1().tag ) );

	notifier.removeNotification( &testClazz, getNotificationTest1() );
	notifier.notify( getNotificationTest1() );
	EXPECT_EQ( 4, testClazz.deliveredCount );
	EXPECT_EQ( 0, notifier.getListenersCount( getNotificationTest1().tag ) );
	EXPECT_EQ( 1, notifier.getListenersCount( getNotificationTest2().tag ) );
}


MAKE_NOTIFICATION( TestUseAfterRelease );

class TestClazzReleased : public CCNode
{
public:
	TestClazzReleased( Notifier& notifier ) :
		m_pNotifier( notifier )
	{
		notifier.addNotification( getNotificationTestUseAfterRelease(), Utils::makeCallback( this,
								  &TestClazzReleased::onCall ) );
	}

	virtual ~TestClazzReleased()
	{
		m_pNotifier.removeAllForObject( this );
	}

	void onCall()
	{
		EXPECT_GT( retainCount() , 0 );  //This object is released?
		CCLOG( "Hello!" );
	}

private:
	Notifier& m_pNotifier;
};

TEST( Notifier, DISABLED_TestOfNotificationUseAfterRelease )
{
	Notifier notifier;

	CCNode* pHolder = new CCNode();
	pHolder->init();
	notifier.addNotification( getNotificationTestUseAfterRelease(), Utils::makeCallback( pHolder,
							  &CCNode::removeAllChildren ) );

	TestClazzReleased* pReleasedNode = new TestClazzReleased( notifier );
	pReleasedNode->init();

	EXPECT_EQ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ( 1, pHolder->retainCount() );

	pHolder->addChild( pReleasedNode );

	EXPECT_EQ( 2, pReleasedNode->retainCount() );
	EXPECT_EQ( 1, pHolder->retainCount() );

	pReleasedNode->release();

	EXPECT_EQ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ( 1, pHolder->retainCount() );

	EXPECT_EQ( 1, pReleasedNode->retainCount() );
	EXPECT_EQ( 1, pHolder->retainCount() );

	//Now when we notify we have problem. We have use after release.
	//It should crash
	notifier.notify( getNotificationTestUseAfterRelease() );

	pHolder->release();
}

