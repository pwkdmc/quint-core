#include <iostream>
#include <limits>
#include "CarSharing/CarSharingManager.h"
#include "CarSharing/exceptions.h"
#include "OnlineCourses/OnlineCoursesManager.h"
#include "OnlineCourses/exceptions.h"
#include "Metro/MetroManager.h"
#include "Metro/exceptions.h"
#include "VendingMachine/VendingMachineManager.h"
#include "VendingMachine/exceptions.h"
#include "Airport/AirportManager.h"
#include "Airport/exceptions.h"

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    std::cout << prompt;
    if (!(std::cin >> value)) {
        clearInputBuffer();
        throw InvalidInputException("Неверный ввод: нужно число");
    }
    clearInputBuffer();
    
    if (value < min || value > max) {
        throw InvalidInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    
    return value;
}

int displayMainMenu() {
    std::cout << "=== ЛАБОРАТОРНАЯ РАБОТА 3 ===\n";
    std::cout << "1. Каршеринг\n";
    std::cout << "2. Онлайн-курсы с разными форматами обучения\n";
    std::cout << "3. Московское метро\n";
    std::cout << "4. Торговый автомат\n";
    std::cout << "5. Аэропорт\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите действие:\n";
    
    return getValidatedInt("", 0, 5);
}

int main() {
    bool running = true;
    while (running) {
        try {
            int choice = displayMainMenu();
            
            switch (choice) {
                case 1: {
                    CarSharingManager* manager = new CarSharingManager();
                    bool inTask = true;
                    int lastAction = 0;
                    
                    while (inTask) {
                        int action = manager->run(lastAction);
                        if (action == 0) {
                            inTask = false;
                        } else {
                            lastAction = action;
                            std::cout << "Задача завершена\n";
                            std::cout << "1. Выполнить задачу еще раз\n";
                            std::cout << "0. Выход\n";
                            bool postChoiceComplete = false;
                            while (!postChoiceComplete) {
                                try {
                                    std::cout << "Выберите действие:\n";
                                    int postChoice = getValidatedInt("", 0, 1);
                                    if (postChoice == 0) {
                                        lastAction = 0;
                                    }
                                    postChoiceComplete = true;
                                } catch (const CarSharingException& e) {
                                    std::cerr << e.what() << "\n";
                                }
                            }
                        }
                    }
                    delete manager;
                    break;
                }
                case 2: {
                    OnlineCoursesManager* manager = new OnlineCoursesManager();
                    bool inTask = true;
                    int lastAction = 0;
                    
                    while (inTask) {
                        int action = manager->run(lastAction);
                        if (action == 0) {
                            inTask = false;
                        } else {
                            lastAction = action;
                            while (true) {
                                try {
                                    std::cout << "Задача завершена\n";
                                    std::cout << "1. Выполнить задачу еще раз\n";
                                    std::cout << "0. Выход\n";
                                    std::cout << "Выберите действие:\n";
                                    int postChoice = getValidatedInt("", 0, 1);
                                    if (postChoice == 0) {
                                        lastAction = 0;
                                    }
                                    break;
                                } catch (const CarSharingException& e) {
                                    std::cerr << e.what() << "\n";
                                }
                            }
                        }
                    }
                    delete manager;
                    break;
                }
                case 3: {
                    MetroManager* manager = new MetroManager();
                    bool inTask = true;
                    int lastAction = 0;
                    
                    while (inTask) {
                        int action = manager->run(lastAction);
                        if (action == 0) {
                            inTask = false;
                        } else {
                            lastAction = action;
                            while (true) {
                                try {
                                    std::cout << "Задача завершена\n";
                                    std::cout << "1. Выполнить задачу еще раз\n";
                                    std::cout << "0. Выход\n";
                                    std::cout << "Выберите действие:\n";
                                    int postChoice = getValidatedInt("", 0, 1);
                                    if (postChoice == 0) {
                                        lastAction = 0;
                                    }
                                    break;
                                } catch (const CarSharingException& e) {
                                    std::cerr << e.what() << "\n";
                                }
                            }
                        }
                    }
                    delete manager;
                    break;
                }
                case 4: {
                    VendingMachineManager* manager = new VendingMachineManager();
                    bool inTask = true;
                    int lastAction = 0;
                    
                    while (inTask) {
                        int action = manager->run(lastAction);
                        if (action == 0) {
                            inTask = false;
                        } else {
                            lastAction = action;
                            while (true) {
                                try {
                                    std::cout << "Задача завершена\n";
                                    std::cout << "1. Выполнить задачу еще раз\n";
                                    std::cout << "0. Выход\n";
                                    std::cout << "Выберите действие:\n";
                                    int postChoice = getValidatedInt("", 0, 1);
                                    if (postChoice == 0) {
                                        lastAction = 0;
                                    }
                                    break;
                                } catch (const CarSharingException& e) {
                                    std::cerr << e.what() << "\n";
                                }
                            }
                        }
                    }
                    delete manager;
                    break;
                }
                case 5: {
                    AirportManager* manager = new AirportManager();
                    bool inTask = true;
                    int lastAction = 0;
                    
                    while (inTask) {
                        int action = manager->run(lastAction);
                        if (action == 0) {
                            inTask = false;
                        } else {
                            lastAction = action;
                            while (true) {
                                try {
                                    std::cout << "Задача завершена\n";
                                    std::cout << "1. Выполнить задачу еще раз\n";
                                    std::cout << "0. Выход\n";
                                    std::cout << "Выберите действие:\n";
                                    int postChoice = getValidatedInt("", 0, 1);
                                    if (postChoice == 0) {
                                        lastAction = 0;
                                    }
                                    break;
                                } catch (const CarSharingException& e) {
                                    std::cerr << e.what() << "\n";
                                }
                            }
                        }
                    }
                    delete manager;
                    break;
                }
                case 0:
                    running = false;
                    break;
            }
        } catch (const CarSharingException& e) {
            std::cerr << e.what() << "\n";
        } catch (const OnlineCoursesException& e) {
            std::cerr << e.what() << "\n";
        } catch (const MetroException& e) {
            std::cerr << e.what() << "\n";
        } catch (const VendingMachineException& e) {
            std::cerr << e.what() << "\n";
        } catch (const AirportException& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
    return 0;
}
