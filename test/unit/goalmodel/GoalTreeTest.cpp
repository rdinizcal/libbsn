#include <gtest/gtest.h>

#include "goalmodel/GoalTree.hpp"
#include "goalmodel/Goal.hpp"
#include "goalmodel/Task.hpp"
#include "goalmodel/LeafTask.hpp"

#include <ostream>
#include <algorithm> // std::find

using namespace bsn::goalmodel;

class GoalTreeTest : public testing::Test { 
    protected:
        GoalTreeTest() {}

        virtual void SetUp() {
        }
};

TEST_F(GoalTreeTest, SimpleConstruct) {
    std::string actor = "Body Sensor Network";

    GoalTree goaltree(actor);

    ASSERT_EQ(goaltree.getActor(), actor);
}

TEST_F(GoalTreeTest, AddRootGoal) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal rgoal("G1", "Emergency is detected");

    goaltree.addRootGoal(rgoal);

    ASSERT_TRUE(goaltree.getNode("G1")==rgoal);
    ASSERT_EQ(goaltree.getSize(), 1);
}

TEST_F(GoalTreeTest, DoNotAllowMoreThanOneRootGoal) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal rgoal1("G1", "Emergency is detected");
    Goal rgoal2("G2", "Patient is healthy");

    goaltree.addRootGoal(rgoal1);

    try {
        goaltree.addRootGoal(rgoal2);
        FAIL() << "Expected not to allow more than one root goal";
    }
    catch(std::invalid_argument const & err) {
        EXPECT_EQ(err.what(),std::string("No more than 1 root goals allowed"));
    }
}

TEST_F(GoalTreeTest, GetSpecificNode) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal rgoal("G1", "Emergency is detected");
    goaltree.addRootGoal(rgoal);

    Node new_goal = goaltree.getNode("G1");

    ASSERT_EQ(Node(rgoal), new_goal);
}

TEST_F(GoalTreeTest, GetTreeSize) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal rgoal("G1", "Emergency is detected");
    goaltree.addRootGoal(rgoal);

    int size = goaltree.getSize();

    ASSERT_TRUE(size == 1);
}

TEST_F(GoalTreeTest, AddGoalWOneChild) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");

    goal1.addChild(goal2);
    goaltree.addRootGoal(goal1);

    ASSERT_EQ(goaltree.getNode("G1"), goal1);
    ASSERT_EQ(goaltree.getNode("G2"), goal2);
    ASSERT_EQ(goaltree.getNode("G1").getChild("G2"), goal2);
}

TEST_F(GoalTreeTest, AddGoalWChildren) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");
    Goal goal3("G3", "Vital signs are monitored");
    Goal goal4("G4", "Vital signs are analyzed");

    goal2.addChild(goal3);
    goal2.addChild(goal4);
    goal1.addChild(goal2);
    goaltree.addRootGoal(goal1);

    ASSERT_EQ(goaltree.getSize(),4);
    ASSERT_TRUE(goaltree.getNode("G2")==goal2);
    ASSERT_EQ(goaltree.getNode("G2").getChildren().size(),2);
    ASSERT_TRUE(goaltree.getNode("G2").getChild("G3")==goal3);
    ASSERT_TRUE(goaltree.getNode("G2").getChild("G4")==goal4);
}

TEST_F(GoalTreeTest, AddGoalWMoreChildren) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");
    Goal goal3("G3", "Vital signs are monitored");
    Goal goal4("G4", "Vital signs are analyzed");
    Goal goal5("G5", "Just another goal");
    Goal goal6("G6", "Other another goal");


    goal5.addChild(goal6);
    goal3.addChild(goal5);
    goal2.addChild(goal4);
    goal2.addChild(goal3);
    goal1.addChild(goal2);

    goaltree.addRootGoal(goal1);

    ASSERT_EQ(goaltree.getSize(),6);
    ASSERT_TRUE(goaltree.getNode("G2").getChild("G3")==goal3);
    //ASSERT_TRUE(goaltree.getNode("G2").getChild("G4")==goal4);
    ASSERT_TRUE(goaltree.getNode("G3").getChild("G5")==goal5);
    ASSERT_TRUE(goaltree.getNode("G5").getChild("G6")==goal6);

}

TEST_F(GoalTreeTest, AddTaskWParent) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");
    Goal goal3("G3", "Vital signs are monitored");
    Goal goal4("G4", "Vital signs are analyzed");

    Task task1("T1", "Analyze vital signs");
    goal4.addChild(task1);
    goal2.addChild(goal4);
    goal2.addChild(goal3);
    goal1.addChild(goal2);
    goaltree.addRootGoal(goal1);

    ASSERT_EQ(goaltree.getNode("T1"), task1);
    ASSERT_EQ(goaltree.getNode("G1"), goal1);
    ASSERT_EQ(goaltree.getNode("G2"), goal2);
    ASSERT_EQ(goaltree.getNode("G3"), goal3);
    ASSERT_EQ(goaltree.getNode("G4"), goal4);

    ASSERT_EQ(goaltree.getNode("G4").getChildren().size(), 1);
    ASSERT_EQ(goaltree.getNode("G4").getChild("T1"), task1);
    ASSERT_EQ(5, goaltree.getSize());
}

TEST_F(GoalTreeTest, AddGoalAsTaskChild) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");
    Goal goal3("G3", "Vital signs are monitored");
    Goal goal4("G4", "Vital signs are analyzed");
    Goal goal5("G5", "The bastard");
    Task task1("T1", "Analyze vital signs");

    try {
        task1.addChild(goal5);
        goal4.addChild(task1);
        goal2.addChild(goal4);
        goal2.addChild(goal3);
        goal1.addChild(goal2);
        goaltree.addRootGoal(goal1);
        FAIL() << "Expected not to allow an insertion of goal as a tasks child";
    }
    catch(std::invalid_argument const & err) {
        EXPECT_EQ(err.what(),std::string("Tasks cannot contain goals as children"));
    }

    ASSERT_TRUE(true);
}

TEST_F(GoalTreeTest, AddTaskWChildren) {
    std::string actor = "Body Sensor Network";
    GoalTree goaltree(actor);
    Goal goal1("G1", "Emergency is detected");
    Goal goal2("G2", "Patient status is monitored");
    Goal goal3("G3", "Vital signs are monitored");
    Goal goal4("G4", "Vital signs are analyzed");
    Task task1("T1", "Analyze vital signs");
    LeafTask task111("T1.11", "Fuse sensors data", Property("W_G4_T1_11",1), Property("R_G4_T1_11",1), Property("F_G4_T1_11",1));
    LeafTask task112("T1.12", "Detect patient status", Property("W_G4_T1_12",1), Property("R_G4_T1_12",1), Property("F_G4_T1_12",1));
    LeafTask task113("T1.13", "Persist patient data", Property("W_G4_T1_13",1), Property("R_G4_T1_13",1), Property("F_G4_T1_13",1));

    task1.addChild(task111);
    task1.addChild(task112);
    task1.addChild(task113);
    goal4.addChild(task1);
    goal2.addChild(goal4);
    goal2.addChild(goal3);
    goal1.addChild(goal2);
    goaltree.addRootGoal(goal1);

    ASSERT_EQ(goaltree.getNode("T1.11"), task111);
    ASSERT_EQ(goaltree.getNode("T1.12"), task112);
    ASSERT_EQ(goaltree.getNode("T1.13"), task113);
    ASSERT_EQ(goaltree.getNode("T1"), task1);
    ASSERT_EQ(goaltree.getNode("G1"), goal1);
    ASSERT_EQ(goaltree.getNode("G2"), goal2);
    ASSERT_EQ(goaltree.getNode("G3"), goal3);
    ASSERT_EQ(goaltree.getNode("G4"), goal4);

    ASSERT_EQ(8, goaltree.getSize());
}

TEST_F(GoalTreeTest, GetLeafTasks) {
    /*Arrange*/
        std::string actor = "Body Sensor Network";
        GoalTree goaltree(actor);
        Goal goal1("G1", "Emergency is detected");
        Goal goal2("G2", "Patient status is monitored");
        Goal goal4("G4", "Vital signs are analyzed");
        Task task1("T1", "Analyze vital signs");
        LeafTask task111("T1.11", "Fuse sensors data", Property("W_G4_T1_11",1), Property("R_G4_T1_11",1), Property("F_G4_T1_11",1));
        LeafTask task112("T1.12", "Detect patient status", Property("W_G4_T1_12",1), Property("R_G4_T1_12",1), Property("F_G4_T1_12",1));
        LeafTask task113("T1.13", "Persist patient data", Property("W_G4_T1_13",1), Property("R_G4_T1_13",1), Property("F_G4_T1_13",1));

        task1.addChild(task111);
        task1.addChild(task112);
        task1.addChild(task113);
        goal4.addChild(task1);
        goal2.addChild(goal4);
        goal1.addChild(goal2);
        goaltree.addRootGoal(goal1);
    
    /*Act*/
        std::vector<Node> LTvec = goaltree.getLeafTasks();

    /*Assert*/
        ASSERT_EQ(3 ,LTvec.size());

        ASSERT_EQ((*std::find(LTvec.begin(), LTvec.end(), task111)), task111);
        ASSERT_EQ((*std::find(LTvec.begin(), LTvec.end(), task112)), task112);
        ASSERT_EQ((*std::find(LTvec.begin(), LTvec.end(), task113)), task113);
}