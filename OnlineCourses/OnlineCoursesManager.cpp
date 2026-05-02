#include <iostream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "OnlineCoursesManager.h"
#include "exceptions.h"

OnlineCoursesManager::OnlineCoursesManager() : dataFile("onlinecourses_data.txt")
{
    std::cout << "\033[1;31mСоздан OnlineCoursesManager\033[0m\n";
    loadFromFile();
}

OnlineCoursesManager::~OnlineCoursesManager()
{

    saveToFile();

    for (size_t i = 0; i < courses.size(); ++i)
    {
        delete courses[i];
    }
    courses.clear();

    for (size_t i = 0; i < students.size(); ++i)
    {
        delete students[i];
    }
    students.clear();
    std::cout << "\033[1;31mУдален OnlineCoursesManager\033[0m\n";
}

int OnlineCoursesManager::getNextCourseId()
{
    int maxId = 0;
    for (size_t i = 0; i < courses.size(); ++i)
    {
        std::string id = courses[i]->getId();
        if (id.length() > 1 && id[0] == 'C')
        {
            try
            {
                int num = std::stoi(id.substr(1));
                if (num > maxId)
                    maxId = num;
            }
            catch (...)
            {
            }
        }
    }
    return maxId + 1;
}

int OnlineCoursesManager::getNextStudentId()
{
    int maxId = 0;
    for (size_t i = 0; i < students.size(); ++i)
    {
        std::string id = students[i]->getId();
        if (id.length() > 1 && id[0] == 'S')
        {
            try
            {
                int num = std::stoi(id.substr(1));
                if (num > maxId)
                    maxId = num;
            }
            catch (...)
            {
            }
        }
    }
    return maxId + 1;
}

int OnlineCoursesManager::getValidatedInt(const std::string &prompt, int min, int max)
{
    int value;
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw OnlineCoursesInputException("Неверный ввод");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (value < min || value > max)
    {
        throw OnlineCoursesInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }

    return value;
}

std::string OnlineCoursesManager::getValidatedString(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty())
    {
        throw OnlineCoursesInputException("Поле не может быть пустым");
    }
    return value;
}

void OnlineCoursesManager::displayCoursesMenu()
{
    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
    }
    else
    {
        std::cout << "= Список курсов =\n";
        for (size_t i = 0; i < courses.size(); ++i)
        {
            std::cout << "---\n";
            std::cout << i + 1 << ". " << courses[i]->getName()
                      << " (ID: " << courses[i]->getId() << ")\n";
        }
        std::cout << "---\n";
    }
}

void OnlineCoursesManager::displayStudentsMenu()
{
    if (students.empty())
    {
        std::cout << "Учеников нет\n";
    }
    else
    {
        std::cout << "= Список учеников =\n";
        for (size_t i = 0; i < students.size(); ++i)
        {
            std::cout << "---\n";
            std::cout << i + 1 << ". " << students[i]->getName()
                      << " (ID: " << students[i]->getId() << ", возраст: "
                      << students[i]->getAge() << " лет)\n";
        }
        std::cout << "---\n";
    }
}

void OnlineCoursesManager::showCourseStatistics()
{
    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
        return;
    }

    std::cout << "= Просмотр курса =\n";
    displayCoursesMenu();
    int courseNum;
    while (true) {
        try {
            std::cout << "Выберите номер курса:\n";
            courseNum = getValidatedInt("", 1, courses.size());
            break;
        } catch (const OnlineCoursesInputException &e) {}
    }

    Course *course = courses[courseNum - 1];

    course->displayInfo();

    std::cout << "Ученики и их прогресс:\n";
    bool hasStudents = false;
    for (size_t i = 0; i < students.size(); ++i)
    {
        const auto &enrolledCourses = students[i]->getEnrolledCourses();
        for (size_t j = 0; j < enrolledCourses.size(); ++j)
        {
            if (enrolledCourses[j] == course->getId())
            {
                hasStudents = true;
                double progress = students[i]->getCourseProgress(course->getId(), course);
                std::cout << students[i]->getName() << ": "
                          << std::fixed << std::setprecision(1) << progress << "%\n";
            }
        }
    }
    if (!hasStudents)
    {
        std::cout << "Учеников нет\n";
    }
}

void OnlineCoursesManager::showStudentStatistics()
{
    if (students.empty())
    {
        std::cout << "Учеников нет\n";
        return;
    }

    std::cout << "= Выбор ученика =\n";
    displayStudentsMenu();
    int studentNum;
    while (true)
    {
        try
        {
            std::cout << "Выберите номер ученика:\n";
            studentNum = getValidatedInt("", 1, students.size());
            break;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    Student *student = students[studentNum - 1];
    student->displayInfo();

    const auto &enrolledCourses = student->getEnrolledCourses();
    if (!enrolledCourses.empty())
    {
        std::cout << "Прогресс по курсам:\n";
        for (size_t i = 0; i < enrolledCourses.size(); ++i)
        {
            std::string courseId = enrolledCourses[i];
            Course *course = nullptr;
            for (size_t j = 0; j < courses.size(); ++j)
            {
                if (courses[j]->getId() == courseId)
                {
                    course = courses[j];
                    break;
                }
            }
            if (course)
            {
                double progress = student->getCourseProgress(courseId, course);
                std::cout << course->getName() << ": "
                          << std::fixed << std::setprecision(1) << progress << "%\n";
            }
        }
    }
}

void OnlineCoursesManager::addStudent()
{
    std::cout << "= Добавить ученика =\n";
    bool complete = false;
    int step = 0;
    std::string name;
    int age = 0;

    while (!complete)
    {
        try
        {
            if (step == 0)
            {
                name = getValidatedString("Введите имя ученика:\n");
                step = 1;
            }
            if (step == 1)
            {
                std::cout << "Введите возраст:\n";
                age = getValidatedInt("", 5, 100);
                step = 2;
            }

            Student *student = new Student("S" + std::to_string(getNextStudentId()), name, age);
            students.push_back(student);
            complete = true;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
            step = 1;
        }
    }
}

void OnlineCoursesManager::removeStudent()
{
    if (students.empty())
    {
        std::cout << "Учеников нет\n";
        return;
    }

    std::cout << "= Удалить ученика =\n";
    bool complete = false;
    int studentNum = 0;
    displayStudentsMenu();

    while (!complete)
    {
        try
        {
            std::cout << "Выберите номер ученика для удаления:\n";
            studentNum = getValidatedInt("", 1, students.size());

            delete students[studentNum - 1];
            students.erase(students.begin() + studentNum - 1);
            complete = true;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

void OnlineCoursesManager::enrollStudentToCourse()
{
    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
        return;
    }

    std::cout << "= Записаться на курс =\n";
    bool complete = false;
    int step = 0;
    int courseNum;
    int choice;
    Student *student;
    std::string name;
    int age;
    while (!complete)
    {
        try
        {
            if (step == 0)
            {
                displayCoursesMenu();
                step = 1;
            }
            if (step == 1)
            {
                std::cout << "Выберите номер курса:\n";
                courseNum = getValidatedInt("", 1, courses.size());
                step = 2;
            }
            if (step == 2)
            {
                std::cout << "1. Выбрать существующего ученика\n";
                std::cout << "2. Добавить нового ученика\n";
                step = 3;
            }
            if (step == 3)
            {
                std::cout << "Выберите действие:\n";
                choice = getValidatedInt("", 1, 2);
                if (choice == 1) step = 4;
                else step = 6;
            }
            if (step == 4) {
                if (choice == 1 && students.empty())
                {
                    std::cout << "Нет существующих учеников\n";
                    step = 6;
                    continue;
                }
                displayStudentsMenu();
                step = 5;
            }
            if (step == 5) {
                std::cout << "Выберите номер ученика:\n";
                int studentNum = getValidatedInt("", 1, students.size());
                student = students[studentNum - 1];
                const auto &enrolledCourses = student->getEnrolledCourses();
                std::string courseId = courses[courseNum - 1]->getId();
                for (size_t i = 0; i < enrolledCourses.size(); ++i)
                {
                    if (enrolledCourses[i] == courseId)
                    {
                        throw StudentAlreadyEnrolledException(student->getId(), courseId);
                    }
                }
                double progress = 0;
                try {
                    progress = student->getCourseProgress(courseId, courses[courseNum - 1]);
                } catch (const OnlineCoursesInputException &e) {
                    progress = 0;
                }
                if (progress >= 100.0)
                {
                    throw CourseAlreadyCompletedException(student->getId(), courseId);
                }
                Course *course = courses[courseNum - 1];
                course->incrementStudentsStarted();
                student->enrollCourse(course->getId(), course->getName());
                std::cout << "Ученик записан на курс " << course->getName() << "\n";
                break;
            }
            if (step == 6) {
                name = getValidatedString("Введите имя ученика:\n");
                step = 7;
            }
            if (step == 7) {
                std::cout << "Введите возраст:\n";
                age = getValidatedInt("", 5, 100);
                student = new Student("S" + std::to_string(getNextStudentId()), name, age);
                students.push_back(student);
                Course *course = courses[courseNum - 1];
                course->incrementStudentsStarted();
                student->enrollCourse(course->getId(), course->getName());
                std::cout << "Ученик записан на курс " << course->getName() << "\n";
                break;
            }
        }
        catch (const StudentAlreadyEnrolledException &e)
        {
            step = 2;
            std::cerr << e.what() << "\n";
        }
        catch (const CourseAlreadyCompletedException &e)
        {
            step = 2;
            std::cerr << e.what() << "\n";
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

void OnlineCoursesManager::completeLesson()
{
    if (students.empty())
    {
        std::cout << "Учеников нет\n";
        return;
    }

    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
        return;
    }

    std::cout << "= Пройти урок =\n";
    bool complete = false;
    int step = -1;
    int studentNum = 0;
    int courseChoice = 0;
    while (!complete)
    {
        try
        {
            if (step == -1)
            {
                displayStudentsMenu();
                step = 0;
            }
            if (step == 0)
            {
                std::cout << "Выберите номер ученика:\n";
                studentNum = getValidatedInt("", 1, students.size());
                step = 1;
            }
            if (step == 1)
            {
                Student *student = students[studentNum - 1];
                auto &enrolledCourses = student->getEnrolledCourses();

                if (enrolledCourses.empty())
                {
                    std::cout << "Ученик не записан ни на один курс\n";
                    complete = true;
                    return;
                }

                std::cout << "Курсы ученика:\n";
                for (size_t i = 0; i < enrolledCourses.size(); ++i)
                {
                    for (size_t j = 0; j < courses.size(); ++j)
                    {
                        if (courses[j]->getId() == enrolledCourses[i])
                        {
                            std::cout << i + 1 << ". " << courses[j]->getName() << "\n";
                            break;
                        }
                    }
                }
                step = 2;
            }
            if (step == 2)
            {
                Student *student = students[studentNum - 1];
                auto &enrolledCourses = student->getEnrolledCourses();
                std::cout << "Выберите номер курса:\n";
                courseChoice = getValidatedInt("", 1, enrolledCourses.size());
                step = 3;
            }
            if (step == 3)
            {
                Student *student = students[studentNum - 1];
                const auto &enrolledCourses = student->getEnrolledCourses();
                std::string courseId = enrolledCourses[courseChoice - 1];
                Course *course = nullptr;
                for (size_t i = 0; i < courses.size(); ++i)
                {
                    if (courses[i]->getId() == courseId)
                    {
                        course = courses[i];
                        break;
                    }
                }

                if (!course)
                {
                    std::cout << "Курс не найден\n";
                    complete = true;
                    return;
                }

                Lesson *nextLesson = student->getNextIncompleteLesson(courseId, course);
                if (!nextLesson)
                {
                    std::cout << "Все уроки курса пройдены\n";
                    complete = true;
                    return;
                }

                std::cout << "Следующий урок:\n";
                nextLesson->displayInfo();

                std::cout << "Описание:\n";
                std::cout << nextLesson->getHomeworkDescription() << "\n";

                std::cout << "1. Пройти урок\n";
                std::cout << "0. Отмена\n";
                std::cout << "Выберите действие:\n";
                int action = getValidatedInt("", 0, 1);

                if (action == 1)
                {
                    student->markLessonComplete(courseId, nextLesson->getId());
                    std::cout << "Урок пройден\n";

                    double progress = student->getCourseProgress(courseId, course);
                    std::cout << "Прогресс курса: " << std::fixed << std::setprecision(1) << progress << "%\n";

                    if (progress >= 100.0)
                    {
                        std::cout << "Поздравляем! Курс пройден полностью\n";
                        course->incrementStudentsCompleted();
                        student->addCertificate(courseId, course->getName());
                    }
                }
                complete = true;
            }
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
            step = -1;
        }
    }
}

void OnlineCoursesManager::addReview()
{
    if (students.empty())
    {
        std::cout << "Учеников нет\n";
        return;
    }

    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
        return;
    }

    std::cout << "= Оставить отзыв =\n";
    bool complete = false;
    int step = -1;
    int studentNum = 0;
    int courseChoice = 0;
    int rating = 0;
    std::string comment;
    Course *course;
    std::string courseId;

    while (!complete)
    {
        try
        {
            if (step == -1)
            {
                displayStudentsMenu();
                step = 0;
            }
            if (step == 0)
            {
                std::cout << "Выберите номер ученика:\n";
                studentNum = getValidatedInt("", 1, students.size());
                step = 1;
            }
            if (step == 1)
            {
                Student *student = students[studentNum - 1];
                const auto &enrolledCourses = student->getEnrolledCourses();

                if (enrolledCourses.empty())
                {
                    std::cout << "Ученик не записан ни на один курс\n";
                    complete = true;
                    return;
                }

                std::cout << "Курсы ученика:\n";
                for (size_t i = 0; i < enrolledCourses.size(); ++i)
                {
                    for (size_t j = 0; j < courses.size(); ++j)
                    {
                        if (courses[j]->getId() == enrolledCourses[i])
                        {
                            std::cout << i + 1 << ". " << courses[j]->getName() << "\n";
                            break;
                        }
                    }
                }
                step = 2;
            }
            if (step == 2)
            {
                Student *student = students[studentNum - 1];
                const auto &enrolledCourses = student->getEnrolledCourses();
                std::cout << "Выберите номер курса:\n";
                courseChoice = getValidatedInt("", 1, enrolledCourses.size());
                step = 3;
            }
            if (step == 3)
            {
                Student *student = students[studentNum - 1];
                const auto &enrolledCourses = student->getEnrolledCourses();
                courseId = enrolledCourses[courseChoice - 1];
                course = nullptr;
                for (size_t i = 0; i < courses.size(); ++i)
                {
                    if (courses[i]->getId() == courseId)
                    {
                        course = courses[i];
                        break;
                    }
                }

                if (!course)
                {
                    std::cout << "Курс не найден\n";
                    complete = true;
                    return;
                }
                step = 4;
            }
            if (step == 4)
            {
                std::cout << "Введите оценку (1-5):\n";
                rating = getValidatedInt("", 1, 5);
                step = 5;
            }
            if (step == 5)
            {
                comment = getValidatedString("Введите комментарий:\n");
                Student *student = students[studentNum - 1];
                course->addReview(student->getId(), student->getName(), rating, comment);
                student->addReview(courseId, course->getName(), rating, comment);
                complete = true;
            }
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

void OnlineCoursesManager::createCourse()
{
    std::cout << "= Создать курс =\n";
    bool complete = false;
    std::string name, description;
    int moduleCount = 0;
    while (true)
    {
        try
        {
            name = getValidatedString("Введите название курса:\n");
            break;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    while (true)
    {
        try
        {
            description = getValidatedString("Введите описание курса:\n");
            break;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    while (true)
    {
        try
        {
            std::cout << "Количество модулей:\n";
            moduleCount = getValidatedInt("", 1, 20);
            break;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    Course *course = new Course("C" + std::to_string(getNextCourseId()), name, description);

    for (int i = 0; i < moduleCount; ++i)
    {
        std::string moduleName;
        int lessonCount = 0;

        while (true)
        {
            try
            {
                moduleName = getValidatedString("Введите название модуля " + std::to_string(i + 1) + ":\n");
                break;
            }
            catch (const OnlineCoursesException &e)
            {
                std::cerr << e.what() << "\n";
                std::cout << "Попробуйте снова\n";
            }
        }

        Module *module = new Module("M" + std::to_string(i + 1), moduleName);

        while (true)
        {
            try
            {
                std::cout << "Количество уроков в модуле " << (i + 1) << ":\n";
                lessonCount = getValidatedInt("", 1, 10);
                break;
            }
            catch (const OnlineCoursesException &e)
            {
                std::cerr << e.what() << "\n";
            }
        }

        for (int j = 0; j < lessonCount; ++j)
        {
            std::string lessonTitle;
            int lessonType = 0;

            while (true)
            {
                try
                {
                    lessonTitle = getValidatedString("Введите название урока " + std::to_string(j + 1) + ":\n");
                    break;
                }
                catch (const OnlineCoursesException &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            std::cout << "1. Видео-лекция\n";
            std::cout << "2. Вебинар\n";
            std::cout << "3. Текстовый материал\n";
            std::cout << "4. Тест с автоматической проверкой\n";
            std::cout << "5. Задание с проверкой преподавателя\n";
            while (true)
            {
                try
                {
                    std::cout << "Выберите тип урока:\n";
                    lessonType = getValidatedInt("", 1, 5);
                    break;
                }
                catch (const OnlineCoursesException &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            Lesson *lesson = nullptr;
            switch (lessonType)
            {
            case 1:
                lesson = new VideoLecture("L" + std::to_string(i) + "_" + std::to_string(j), lessonTitle);
                break;
            case 2:
                lesson = new Webinar("L" + std::to_string(i) + "_" + std::to_string(j), lessonTitle);
                break;
            case 3:
                lesson = new TextMaterial("L" + std::to_string(i) + "_" + std::to_string(j), lessonTitle);
                break;
            case 4:
                lesson = new TestAssignment("L" + std::to_string(i) + "_" + std::to_string(j), lessonTitle);
                break;
            case 5:
                lesson = new TeacherCheckedAssignment("L" + std::to_string(i) + "_" + std::to_string(j), lessonTitle);
                break;
            }

            if (lesson)
                module->addLesson(lesson);
        }

        course->addModule(module);
    }

    courses.push_back(course);
}

void OnlineCoursesManager::removeCourse()
{
    if (courses.empty())
    {
        std::cout << "Курсов нет\n";
        return;
    }

    std::cout << "= Удалить курс =\n";
    bool complete = false;
    int courseNum = 0;
    displayCoursesMenu();
    while (!complete)
    {
        try
        {
            std::cout << "Выберите номер курса для удаления:\n";
            courseNum = getValidatedInt("", 1, courses.size());

            Course* course = courses[courseNum - 1];
            std::string courseId = course->getId();
            
            for (size_t i = 0; i < students.size(); ++i)
            {
                students[i]->removeCourse(courseId);
                students[i]->removeReviewsForCourse(courseId);
            }

            delete course;
            courses.erase(courses.begin() + courseNum - 1);
            std::cout << "Курс удален\n";
            complete = true;
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
}

int OnlineCoursesManager::run(int lastAction)
{
    bool taskComplete = false;
    int actionToRepeat = lastAction;

    while (!taskComplete)
    {
        if (lastAction == 0)
        {
            std::cout << "== ОНЛАЙН-КУРСЫ ==\n";
            std::cout << "1. Просмотреть курс\n";
            std::cout << "2. Просмотреть статистику ученика\n";
            std::cout << "3. Добавить ученика\n";
            std::cout << "4. Удалить ученика\n";
            std::cout << "5. Записаться на курс\n";
            std::cout << "6. Пройти урок\n";
            std::cout << "7. Оставить отзыв\n";
            std::cout << "8. Создать курс\n";
            std::cout << "9. Удалить курс\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие:\n";
        }

        try
        {
            if (lastAction == 0)
            {
                lastAction = getValidatedInt("", 0, 10);
            }

            switch (lastAction)
            {
            case 1:
                showCourseStatistics();
                taskComplete = true;
                break;
            case 2:
                showStudentStatistics();
                taskComplete = true;
                break;
            case 3:
                addStudent();
                saveToFile();
                taskComplete = true;
                break;
            case 4:
                removeStudent();
                saveToFile();
                taskComplete = true;
                break;
            case 5:
                enrollStudentToCourse();
                saveToFile();
                taskComplete = true;
                break;
            case 6:
                completeLesson();
                saveToFile();
                taskComplete = true;
                break;
            case 7:
                addReview();
                saveToFile();
                taskComplete = true;
                break;
            case 8:
                createCourse();
                saveToFile();
                taskComplete = true;
                break;
            case 9:
                removeCourse();
                saveToFile();
                taskComplete = true;
                break;
            case 0:
                return 0;
            }
        }
        catch (const OnlineCoursesException &e)
        {
            std::cerr << e.what() << "\n";
            lastAction = actionToRepeat;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Ошибка: " << e.what() << "\n";
            lastAction = actionToRepeat;
        }
    }

    return lastAction;
}

bool OnlineCoursesManager::saveToFile() const
{
    std::ofstream file(dataFile);
    if (!file.is_open())
    {
        return false;
    }

    file << "COURSE_COUNT:" << courses.size() << "\n";
    for (size_t i = 0; i < courses.size(); ++i)
    {
        Course *course = courses[i];
        file << "COURSE_START\n";
        file << "ID:" << course->getId() << "\n";
        file << "NAME:" << course->getName() << "\n";
        file << "DESC:" << course->getDescription() << "\n";
        file << "STARTED:" << course->getStudentsStarted() << "\n";
        file << "COMPLETED:" << course->getStudentsCompleted() << "\n";

        file << "MODULE_COUNT:" << course->getModules().size() << "\n";
        const auto &modules = course->getModules();
        for (size_t m = 0; m < modules.size(); ++m)
        {
            file << "MODULE_START\n";
            file << "MID:" << modules[m]->getId() << "\n";
            file << "MNAME:" << modules[m]->getName() << "\n";

            const auto &lessons = modules[m]->getLessons();
            file << "LESSON_COUNT:" << lessons.size() << "\n";
            for (size_t l = 0; l < lessons.size(); ++l)
            {
                file << "LESSON_START\n";
                file << "LID:" << lessons[l]->getId() << "\n";
                file << "LTITLE:" << lessons[l]->getTitle() << "\n";
                file << "LTYPE:" << lessons[l]->getType() << "\n";
                file << "LESSON_END\n";
            }
            file << "MODULE_END\n";
        }

        const auto &reviews = course->getReviews();
        file << "REVIEW_COUNT:" << reviews.size() << "\n";
        for (size_t r = 0; r < reviews.size(); ++r)
        {
            file << "REVIEW_START\n";
            file << "RSTUDENTID:" << reviews[r].studentId << "\n";
            file << "RSTUDENTNAME:" << reviews[r].studentName << "\n";
            file << "RRATING:" << reviews[r].rating << "\n";
            file << "RCOMMENT:" << reviews[r].comment << "\n";
            file << "REVIEW_END\n";
        }
        file << "COURSE_END\n";
    }

    file << "STUDENT_COUNT:" << students.size() << "\n";
    for (size_t i = 0; i < students.size(); ++i)
    {
        Student *student = students[i];
        file << "STUDENT_START\n";
        file << "SID:" << student->getId() << "\n";
        file << "SNAME:" << student->getName() << "\n";
        file << "SAGE:" << student->getAge() << "\n";

        const auto &enrolledCourses = student->getEnrolledCourses();
        file << "ENROLLED_COUNT:" << enrolledCourses.size() << "\n";
        for (size_t c = 0; c < enrolledCourses.size(); ++c)
        {
            file << "ENROLLED:" << enrolledCourses[c] << "\n";
        }

        for (size_t c = 0; c < enrolledCourses.size(); ++c)
        {
            const std::string &courseId = enrolledCourses[c];
            for (size_t m = 0; m < courses.size(); ++m)
            {
                if (courses[m]->getId() == courseId)
                {
                    const auto &modules = courses[m]->getModules();
                    for (size_t mod = 0; mod < modules.size(); ++mod)
                    {
                        const auto &lessons = modules[mod]->getLessons();
                        for (size_t l = 0; l < lessons.size(); ++l)
                        {
                            if (student->isLessonComplete(courseId, lessons[l]->getId()))
                            {
                                file << "PROGRESS:" << courseId << "|" << lessons[l]->getId() << "\n";
                            }
                        }
                    }
                    break;
                }
            }
        }

        const auto &certs = student->getCertificates();
        file << "CERT_COUNT:" << certs.size() << "\n";
        for (size_t c = 0; c < certs.size(); ++c)
        {
            file << "CERT:" << certs[c].courseId << "|" << certs[c].courseName << "\n";
        }

        const auto &sReviews = student->getReviews();
        file << "SREVIEW_COUNT:" << sReviews.size() << "\n";
        for (size_t r = 0; r < sReviews.size(); ++r)
        {
            file << "SREVIEW:" << sReviews[r].courseId << "|" << sReviews[r].courseName << "|"
                 << sReviews[r].rating << "|" << sReviews[r].comment << "\n";
        }
        file << "STUDENT_END\n";
    }

    file.close();
    return true;
}

bool OnlineCoursesManager::loadFromFile()
{
    std::ifstream file(dataFile);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    std::string currentCourseId;
    std::string currentStudentId;

    while (std::getline(file, line))
    {
        if (line.substr(0, 12) == "COURSE_COUNT")
        {
        }
        else if (line.substr(0, 13) == "STUDENT_COUNT")
        {
        }
        else if (line.substr(0, 12) == "COURSE_START")
        {
        }
        else if (line.substr(0, 10) == "COURSE_END")
        {
        }
        else if (line.substr(0, 3) == "ID:")
        {
            std::string id = line.substr(3);
            std::string name, desc;
            int started = 0, completed = 0;

            while (std::getline(file, line))
            {
                if (line.substr(0, 5) == "NAME:")
                    name = line.substr(5);
                else if (line.substr(0, 5) == "DESC:")
                    desc = line.substr(5);
                else if (line.substr(0, 8) == "STARTED:")
                    started = std::stoi(line.substr(8));
                else if (line.substr(0, 10) == "COMPLETED:")
                    completed = std::stoi(line.substr(10));
                else if (line.substr(0, 13) == "MODULE_COUNT:")
                {
                    int moduleCount = std::stoi(line.substr(13));
                    Course *course = new Course(id, name, desc, started, completed);
                    for (int i = 0; i < moduleCount; ++i)
                    {
                        std::getline(file, line);
                        std::getline(file, line);
                        std::string moduleId = line.substr(4);
                        std::getline(file, line);
                        std::string moduleName = line.substr(6);
                        Module *module = new Module(moduleId, moduleName);

                        std::getline(file, line);
                        int lessonCount = std::stoi(line.substr(13));
                        for (int j = 0; j < lessonCount; ++j)
                        {
                            std::getline(file, line);
                            std::getline(file, line);
                            std::string lessonId = line.substr(4);
                            std::getline(file, line);
                            std::string lessonTitle = line.substr(7);
                            std::getline(file, line);
                            std::string lessonType = line.substr(6);

                            Lesson *lesson = nullptr;
                            if (lessonType == "Видео-лекция")
                            {
                                lesson = new VideoLecture(lessonId, lessonTitle);
                            }
                            else if (lessonType == "Вебинар")
                            {
                                lesson = new Webinar(lessonId, lessonTitle);
                            }
                            else if (lessonType == "Текстовый материал")
                            {
                                lesson = new TextMaterial(lessonId, lessonTitle);
                            }
                            else if (lessonType.find("Тест") != std::string::npos)
                            {
                                lesson = new TestAssignment(lessonId, lessonTitle);
                            }
                            else if (lessonType.find("Задание") != std::string::npos)
                            {
                                lesson = new TeacherCheckedAssignment(lessonId, lessonTitle);
                            }
                            if (lesson)
                                module->addLesson(lesson);

                            std::getline(file, line);
                        }
                        std::getline(file, line);
                        course->addModule(module);
                    }
                    courses.push_back(course);
                    currentCourseId = id;
                    break;
                }
            }
        }
        else if (line.substr(0, 12) == "REVIEW_START")
        {
            int rating = 0;
            std::string studentId, studentName, comment;
            while (std::getline(file, line))
            {
                if (line.substr(0, 11) == "RSTUDENTID:")
                    studentId = line.substr(12);
                else if (line.substr(0, 13) == "RSTUDENTNAME:")
                    studentName = line.substr(13);
                else if (line.substr(0, 8) == "RRATING:")
                    rating = std::stoi(line.substr(8));
                else if (line.substr(0, 9) == "RCOMMENT:")
                    comment = line.substr(9);
                else if (line.substr(0, 10) == "REVIEW_END")
                {
                    for (size_t i = 0; i < courses.size(); ++i)
                    {
                        if (courses[i]->getId() == currentCourseId)
                        {
                            courses[i]->addReview(studentId, studentName, rating, comment);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        else if (line.substr(0, 13) == "STUDENT_START")
        {
            std::string id, name;
            int age = 0;
            while (std::getline(file, line))
            {
                if (line.substr(0, 4) == "SID:")
                    id = line.substr(4);
                else if (line.substr(0, 6) == "SNAME:")
                    name = line.substr(6);
                else if (line.substr(0, 5) == "SAGE:")
                    age = std::stoi(line.substr(5));
                else if (line.substr(0, 15) == "ENROLLED_COUNT:")
                {
                    int enrolledCount = std::stoi(line.substr(15));
                    Student *student = new Student(id, name, age);
                    students.push_back(student);
                    currentStudentId = id;

                    for (int i = 0; i < enrolledCount; ++i)
                    {
                        std::getline(file, line);
                        std::string courseId = line.substr(9);
                        for (size_t c = 0; c < courses.size(); ++c)
                        {
                            if (courses[c]->getId() == courseId)
                            {
                                student->enrollCourse(courseId, courses[c]->getName());
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        else if (line.substr(0, 8) == "PROGRESS")
        {
            std::string data = line.substr(9);
            size_t pos = data.find("|");
            if (pos != std::string::npos)
            {
                std::string courseId = data.substr(0, pos);
                std::string lessonId = data.substr(pos + 1);
                for (size_t i = 0; i < students.size(); ++i)
                {
                    if (students[i]->getId() == currentStudentId)
                    {
                        students[i]->markLessonComplete(courseId, lessonId);
                        break;
                    }
                }
            }
        }
        else if (line.substr(0, 5) == "CERT:")
        {
            std::string data = line.substr(5);
            size_t pos1 = data.find("|");
            size_t pos2 = data.find("|", pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos)
            {
                std::string courseId = data.substr(0, pos1);
                std::string courseName = data.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string issueDate = data.substr(pos2 + 1);
                for (size_t i = 0; i < students.size(); ++i)
                {
                    if (students[i]->getId() == currentStudentId)
                    {
                        students[i]->addCertificate(courseId, courseName);
                        break;
                    }
                }
            }
        }
        else if (line.substr(0, 8) == "SREVIEW:")
        {
            std::string data = line.substr(8);
            size_t pos1 = data.find("|");
            size_t pos2 = data.find("|", pos1 + 1);
            size_t pos3 = data.find("|", pos2 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos)
            {
                std::string courseId = data.substr(0, pos1);
                std::string courseName = data.substr(pos1 + 1, pos2 - pos1 - 1);
                int rating = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
                std::string comment = data.substr(pos3 + 1);
                for (size_t i = 0; i < students.size(); ++i)
                {
                    if (students[i]->getId() == currentStudentId)
                    {
                        students[i]->addReview(courseId, courseName, rating, comment);
                        break;
                    }
                }
            }
        }
        else if (line.substr(0, 11) == "STUDENT_END")
        {
            currentStudentId = "";
        }
    }

    file.close();
    return true;
}
