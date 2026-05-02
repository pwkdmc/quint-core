#ifndef ONLINECOURSES_MANAGER_H
#define ONLINECOURSES_MANAGER_H

#include <vector>
#include <string>
#include "Course.h"
#include "Student.h"
#include "Module.h"
#include "Lesson.h"

class OnlineCoursesManager {
private:
    std::vector<Course*> courses;
    std::vector<Student*> students;
    std::string dataFile;
    
    void displayCoursesMenu();
    void displayStudentsMenu();
    int getValidatedInt(const std::string& prompt, int min, int max);
    std::string getValidatedString(const std::string& prompt);
    int getNextCourseId();
    int getNextStudentId();
    bool saveToFile() const;
    bool loadFromFile();
    
public:
    OnlineCoursesManager();
    ~OnlineCoursesManager();
    
    int run(int lastAction = 0);
    
private:
    void showCourseStatistics();
    void showStudentStatistics();
    void addStudent();
    void removeStudent();
    void enrollStudentToCourse();
    void completeLesson();
    void showProgress();
    void addReview();
    void createCourse();
    void removeCourse();
};

#endif
