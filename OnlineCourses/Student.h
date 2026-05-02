#ifndef ONLINECOURSES_STUDENT_H
#define ONLINECOURSES_STUDENT_H

#include <string>
#include <vector>
#include <map>
#include "Course.h"
#include "exceptions.h"

struct Certificate {
    std::string courseId;
    std::string courseName;
};

struct StudentReview {
    std::string courseId;
    std::string courseName;
    int rating;
    std::string comment;
};

class Student {
private:
    std::string id;
    std::string name;
    int age;
    std::vector<std::string> enrolledCourses;
    std::map<std::string, std::map<std::string, bool>> courseLessonProgress;
    std::vector<Certificate> certificates;
    std::vector<StudentReview> reviews;
    
public:
    Student(const std::string& id, const std::string& name, int age)
        : id(id), name(name), age(age) {
        std::cout << "\033[1;31mСоздан Student\033[0m\n";
    }
    
    ~Student() {
        std::cout << "\033[1;31mУдален Student\033[0m\n";
    }
    
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getAge() const { return age; }
    
    void enrollCourse(const std::string& courseId, const std::string& courseName) {
        for (size_t i = 0; i < enrolledCourses.size(); ++i) {
            if (enrolledCourses[i] == courseId) {
                return;
            }
        }
        enrolledCourses.push_back(courseId);
        courseLessonProgress[courseId] = std::map<std::string, bool>();
    }
    
    void removeCourse(const std::string& courseId) {
        for (auto it = enrolledCourses.begin(); it != enrolledCourses.end(); ++it) {
            if (*it == courseId) {
                enrolledCourses.erase(it);
                courseLessonProgress.erase(courseId);
                break;
            }
        }
    }
    
    const std::vector<std::string>& getEnrolledCourses() const {
        return enrolledCourses;
    }
    
    void markLessonComplete(const std::string& courseId, const std::string& lessonId) {
        if (!courseLessonProgress.count(courseId)) {
            throw OnlineCoursesInputException("Курс не найден у ученика");
        }
        courseLessonProgress[courseId][lessonId] = true;
    }
    
    bool isLessonComplete(const std::string& courseId, const std::string& lessonId) const {
        if (!courseLessonProgress.count(courseId)) {
            return false;
        }
        const auto& lessons = courseLessonProgress.at(courseId);
        if (!lessons.count(lessonId)) {
            return false;
        }
        return lessons.at(lessonId);
    }
    
    double getCourseProgress(const std::string& courseId, const Course* course) const {
        if (!courseLessonProgress.count(courseId)) {
            throw OnlineCoursesInputException("Курс не найден у ученика");
        }
        return course->calculateCourseProgress(courseLessonProgress);
    }
    
    void addCertificate(const std::string& courseId, const std::string& courseName) {
        Certificate cert;
        cert.courseId = courseId;
        cert.courseName = courseName;
        certificates.push_back(cert);
    }
    
    const std::vector<Certificate>& getCertificates() const {
        return certificates;
    }
    
    void addReview(const std::string& courseId, const std::string& courseName, 
                   int rating, const std::string& comment) {
        if (rating < 1 || rating > 5) {
            throw InvalidRatingException(rating);
        }
        StudentReview review;
        review.courseId = courseId;
        review.courseName = courseName;
        review.rating = rating;
        review.comment = comment;
        reviews.push_back(review);
    }
    
    void removeReviewsForCourse(const std::string& courseId) {
        for (auto it = reviews.begin(); it != reviews.end(); ++it) {
            if (it->courseId == courseId) {
                reviews.erase(it);
            }
        }
    }
    
    const std::vector<StudentReview>& getReviews() const {
        return reviews;
    }
    
    Lesson* getNextIncompleteLesson(const std::string& courseId, const Course* course) const {
        return course->getNextIncompleteLesson(id, courseLessonProgress);
    }
    
    void displayInfo() const {
        std::cout << "= Ученик: " << name << " =\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "Возраст: " << age << " лет\n";
        std::cout << "Записан на курсы:\n";
        
        if (enrolledCourses.empty()) {
            std::cout << "Нет записей на курсы\n";
        } else {
            for (size_t i = 0; i < enrolledCourses.size(); ++i) {
                std::cout << i + 1 << ". " << enrolledCourses[i] << "\n";
            }
        }
        
        if (!certificates.empty()) {
            std::cout << "Сертификаты:\n";
            for (size_t i = 0; i < certificates.size(); ++i) {
                std::cout << i + 1 << ". " << certificates[i].courseName << "\n";
            }
        }
        
        if (!reviews.empty()) {
            std::cout << "Написанные отзывы:\n";
            for (size_t i = 0; i < reviews.size(); ++i) {
                std::cout << i + 1 << ". " << reviews[i].courseName 
                          << " - Оценка: " << reviews[i].rating << "/5\n";
                std::cout << "Комментарий: " << reviews[i].comment << "\n";
            }
        }
    }
};

#endif
