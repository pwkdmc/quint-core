#ifndef ONLINECOURSES_COURSE_H
#define ONLINECOURSES_COURSE_H

#include <string>
#include <vector>
#include <map>
#include "Module.h"
#include "exceptions.h"

struct CourseReview {
    std::string studentId;
    std::string studentName;
    int rating;
    std::string comment;
};

class Course {
private:
    std::string id;
    std::string name;
    std::string description;
    std::vector<Module*> modules;
    std::vector<CourseReview> reviews;
    int studentsStarted;
    int studentsCompleted;
    
public:
    Course(const std::string& id, const std::string& name, const std::string& description, const int studentsStarted = 0, const int studentsCompleted = 0)
        : id(id), name(name), description(description), studentsStarted(studentsStarted), studentsCompleted(studentsCompleted) {
        std::cout << "\033[1;31mСоздан Course\033[0m\n";
    }
    
    ~Course() {
        std::cout << "\033[1;31mУдален Course\033[0m\n";
        for (size_t i = 0; i < modules.size(); ++i) {
            delete modules[i];
        }
        modules.clear();
    }
    
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    
    void addModule(Module* module) {
        modules.push_back(module);
    }
    
    const std::vector<Module*>& getModules() const {
        return modules;
    }
    
    void addReview(const std::string& studentId, const std::string& studentName, 
                   int rating, const std::string& comment) {
        if (rating < 1 || rating > 5) {
            throw InvalidRatingException(rating);
        }
        CourseReview review;
        review.studentId = studentId;
        review.studentName = studentName;
        review.rating = rating;
        review.comment = comment;
        reviews.push_back(review);
    }
    
    const std::vector<CourseReview>& getReviews() const {
        return reviews;
    }
    
    void incrementStudentsStarted() {
        studentsStarted++;
    }
    
    void incrementStudentsCompleted() {
        studentsCompleted++;
    }
    
    int getStudentsStarted() const { return studentsStarted; }
    int getStudentsCompleted() const { return studentsCompleted; }
    
    double calculateCourseProgress(const std::map<std::string, std::map<std::string, bool>>& studentLessonProgress) const {
        if (modules.empty()) return 0.0;
        
        double totalWeight = 0.0;
        double completedWeight = 0.0;
        
        for (size_t i = 0; i < modules.size(); ++i) {
            const auto& lessons = modules[i]->getLessons();
            for (size_t j = 0; j < lessons.size(); ++j) {
                double weight = lessons[j]->getProgressWeight();
                totalWeight += weight;
                
                std::string lessonId = lessons[j]->getId();
                if (studentLessonProgress.count(id) && 
                    studentLessonProgress.at(id).count(lessonId) && 
                    studentLessonProgress.at(id).at(lessonId)) {
                    completedWeight += weight;
                }
            }
        }
        
        if (totalWeight == 0) return 0.0;
        return (completedWeight / totalWeight) * 100.0;
    }
    
    Lesson* getNextIncompleteLesson(const std::string& studentId, 
                                     const std::map<std::string, std::map<std::string, bool>>& studentLessonProgress) const {
        for (size_t i = 0; i < modules.size(); ++i) {
            const auto& lessons = modules[i]->getLessons();
            for (size_t j = 0; j < lessons.size(); ++j) {
                std::string lessonId = lessons[j]->getId();
                if (!studentLessonProgress.count(id) || 
                    !studentLessonProgress.at(id).count(lessonId) || 
                    !studentLessonProgress.at(id).at(lessonId)) {
                    return lessons[j];
                }
            }
        }
        return nullptr;
    }
    
    void displayInfo() const {
        std::cout << "= Курс: " << name << " =\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Описание: " << description << "\n";
        std::cout << "Модулей: " << modules.size() << "\n";
        std::cout << "Начали курс: " << studentsStarted << " человек\n";
        std::cout << "Закончили курс: " << studentsCompleted << " человек\n";
        
        if (!reviews.empty()) {
            std::cout << "Отзывы:\n";
            for (size_t i = 0; i < reviews.size(); ++i) {
                std::cout << i + 1 << ". " << reviews[i].studentName 
                          << " - Оценка: " << reviews[i].rating << "/5\n";
                std::cout << "Комментарий: " << reviews[i].comment << "\n";
            }
        }
        
        for (size_t i = 0; i < modules.size(); ++i) {
            std::cout << "---\n";
            modules[i]->displayInfo();
        }
        std::cout << "---\n";
    }
};

#endif
