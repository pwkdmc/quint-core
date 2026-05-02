#ifndef ONLINECOURSES_LESSON_H
#define ONLINECOURSES_LESSON_H

#include <string>
#include <vector>
#include "exceptions.h"

class Lesson {
protected:
    std::string id;
    std::string title;
    std::string type;
    
public:
    Lesson(const std::string& id, const std::string& title, const std::string& type)
        : id(id), title(title), type(type) {
        std::cout << "\033[1;31mСоздан Lesson\033[0m\n";
    }
    
    virtual ~Lesson() {
        std::cout << "\033[1;31mУдален Lesson\033[0m\n";
    }
    
    virtual double getProgressWeight() const = 0;
    virtual std::string getLessonType() const = 0;
    virtual std::string getHomeworkDescription() const = 0;
    
    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getType() const { return type; }
    
    virtual void displayInfo() const {
        std::cout << "ID: " << id << "\n";
        std::cout << "Название: " << title << "\n";
        std::cout << "Тип: " << type << "\n";
        std::cout << "Вес в прогрессе: " << getProgressWeight() << "%\n";
    }
};

class VideoLecture : public Lesson {
public:
    VideoLecture(const std::string& id, const std::string& title)
        : Lesson(id, title, "Видео-лекция") {
        std::cout << "\033[1;31mСоздан VideoLecture\033[0m\n";
    }
    
    ~VideoLecture() override {
        std::cout << "\033[1;31mУдален VideoLecture\033[0m\n";
    }
    
    double getProgressWeight() const override { return 15.0; }
    std::string getLessonType() const override { return "Видео-лекция"; }
    std::string getHomeworkDescription() const override {
        return "Просмотреть записанную лекцию и законспектировать ключевые моменты";
    }
};

class Webinar : public Lesson {
public:
    Webinar(const std::string& id, const std::string& title)
        : Lesson(id, title, "Вебинар") {
        std::cout << "\033[1;31mСоздан Webinar\033[0m\n";
    }
    
    ~Webinar() override {
        std::cout << "\033[1;31mУдален Webinar\033[0m\n";
    }
    
    double getProgressWeight() const override { return 20.0; }
    std::string getLessonType() const override { return "Вебинар"; }
    std::string getHomeworkDescription() const override {
        return "Принять участие в онлайн-занятии с преподавателем и задать вопросы";
    }
};

class TextMaterial : public Lesson {
public:
    TextMaterial(const std::string& id, const std::string& title)
        : Lesson(id, title, "Текстовый материал") {
        std::cout << "\033[1;31mСоздан TextMaterial\033[0m\n";
    }
    
    ~TextMaterial() override {
        std::cout << "\033[1;31mУдален TextMaterial\033[0m\n";
    }
    
    double getProgressWeight() const override { return 10.0; }
    std::string getLessonType() const override { return "Текстовый материал"; }
    std::string getHomeworkDescription() const override {
        return "Изучить учебный текст и ответить на контрольные вопросы";
    }
};

class InteractiveAssignment : public Lesson {
protected:
    std::string assignmentType;
    
public:
    InteractiveAssignment(const std::string& id, const std::string& title, const std::string& assignmentType)
        : Lesson(id, title, "Задание"), assignmentType(assignmentType) {
        std::cout << "\033[1;31mСоздан InteractiveAssignment\033[0m\n";
    }
    
    ~InteractiveAssignment() override {
        std::cout << "\033[1;31mУдален InteractiveAssignment\033[0m\n";
    }
    
    double getProgressWeight() const override { return 25.0; }
    std::string getLessonType() const override { return "Задание (" + assignmentType + ")"; }
    
    void displayInfo() const override {
        Lesson::displayInfo();
        std::cout << "Тип задания: " << assignmentType << "\n";
    }
};

class TestAssignment : public InteractiveAssignment {
public:
    TestAssignment(const std::string& id, const std::string& title)
        : InteractiveAssignment(id, title, "Тест с автоматической проверкой") {
        std::cout << "\033[1;31mСоздан TestAssignment\033[0m\n";
    }
    
    ~TestAssignment() override {
        std::cout << "\033[1;31mУдален TestAssignment\033[0m\n";
    }
    
    std::string getHomeworkDescription() const override {
        return "Пройти тест с автоматической проверкой. Результаты будут показаны сразу после завершения";
    }
};

class TeacherCheckedAssignment : public InteractiveAssignment {
public:
    TeacherCheckedAssignment(const std::string& id, const std::string& title)
        : InteractiveAssignment(id, title, "Задание с проверкой преподавателя") {
        std::cout << "\033[1;31mСоздан TeacherCheckedAssignment\033[0m\n";
    }
    
    ~TeacherCheckedAssignment() override {
        std::cout << "\033[1;31mУдален TeacherCheckedAssignment\033[0m\n";
    }
    
    std::string getHomeworkDescription() const override {
        return "Выполнить практическое задание. Проверка преподавателем занимает до 3 рабочих дней";
    }
};

#endif
