#ifndef ONLINECOURSES_EXCEPTIONS_H
#define ONLINECOURSES_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class OnlineCoursesException : public std::runtime_error {
public:
    explicit OnlineCoursesException(const std::string& message) 
        : std::runtime_error(message) {}
};

class OnlineCoursesInputException : public OnlineCoursesException {
public:
    explicit OnlineCoursesInputException(const std::string& message) 
        : OnlineCoursesException("Ошибка: " + message) {}
};

class CourseNotFoundException : public OnlineCoursesException {
public:
    explicit CourseNotFoundException(const std::string& courseId) 
        : OnlineCoursesException("Ошибка: Курс " + courseId + " не найден") {}
};

class StudentNotFoundException : public OnlineCoursesException {
public:
    explicit StudentNotFoundException(const std::string& studentId) 
        : OnlineCoursesException("Ошибка: Ученик " + studentId + " не найден") {}
};

class InvalidRatingException : public OnlineCoursesException {
public:
    explicit InvalidRatingException(int rating) 
        : OnlineCoursesException("Ошибка: Оценка должна быть от 1 до 5, получено: " + std::to_string(rating)) {}
};

class InvalidStudentAgeException : public OnlineCoursesException {
public:
    explicit InvalidStudentAgeException(int age) 
        : OnlineCoursesException("Ошибка: Неверный возраст: " + std::to_string(age)) {}
};

class StudentAlreadyEnrolledException : public OnlineCoursesException {
public:
    explicit StudentAlreadyEnrolledException(const std::string& studentId, const std::string& courseId) 
        : OnlineCoursesException("Ошибка: Ученик " + studentId + " уже записан на курс " + courseId) {}
};

class CourseAlreadyCompletedException : public OnlineCoursesException {
public:
    explicit CourseAlreadyCompletedException(const std::string& studentId, const std::string& courseId) 
        : OnlineCoursesException("Ошибка: Ученик " + studentId + " уже закончил курс " + courseId) {}
};

#endif
