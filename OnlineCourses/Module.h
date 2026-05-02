#ifndef ONLINECOURSES_MODULE_H
#define ONLINECOURSES_MODULE_H

#include <string>
#include <vector>
#include <map>
#include "Lesson.h"

class Module {
private:
    std::string id;
    std::string name;
    std::vector<Lesson*> lessons;
    
public:
    Module(const std::string& id, const std::string& name)
        : id(id), name(name) {
        std::cout << "\033[1;31mСоздан Module\033[0m\n";
    }
    
    ~Module() {
        std::cout << "\033[1;31mУдален Module\033[0m\n";
        for (size_t i = 0; i < lessons.size(); ++i) {
            delete lessons[i];
        }
        lessons.clear();
    }
    
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    
    void addLesson(Lesson* lesson) {
        lessons.push_back(lesson);
    }
    
    const std::vector<Lesson*>& getLessons() const {
        return lessons;
    }
    
    double calculateProgress(const std::map<std::string, bool>& lessonProgress) const {
        if (lessons.empty()) return 0.0;
        
        double totalWeight = 0.0;
        double completedWeight = 0.0;
        
        for (size_t i = 0; i < lessons.size(); ++i) {
            double weight = lessons[i]->getProgressWeight();
            totalWeight += weight;
            std::string lessonId = lessons[i]->getId();
            if (lessonProgress.count(lessonId) && lessonProgress.at(lessonId)) {
                completedWeight += weight;
            }
        }
        
        if (totalWeight == 0) return 0.0;
        return (completedWeight / totalWeight) * 100.0;
    }
    
    Lesson* getNextIncompleteLesson(const std::map<std::string, bool>& lessonProgress) const {
        for (size_t i = 0; i < lessons.size(); ++i) {
            std::string lessonId = lessons[i]->getId();
            if (!lessonProgress.count(lessonId) || !lessonProgress.at(lessonId)) {
                return lessons[i];
            }
        }
        return nullptr;
    }
    
    void displayInfo() const {
        std::cout << "= Модуль: " << name << " =\n";
        std::cout << "Уроков: " << lessons.size() << "\n";
        for (size_t i = 0; i < lessons.size(); ++i) {
            std::cout << "---\n";
            lessons[i]->displayInfo();
        }
        std::cout << "---\n";
    }
};

#endif
