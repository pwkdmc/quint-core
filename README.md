# Multi-System Management Suite 🚀
A feature-rich C++ console application that integrates five independent management systems into a single unified interface. Built with object-oriented design principles, polymorphism, custom exception handling, and persistent file storage.
## 🌟 Overview
This project simulates real-world complex systems with deep class hierarchies (inheritance, virtual functions), robust input validation, state management, and JSON-like serialization to text files. Each subsystem operates independently with its own domain model, business logic, and custom exception types.
## 🧩 Subsystems
### 🚗 Car Sharing
A car rental platform supporting multiple vehicle categories (`EconomyCar`, `BusinessCar`, `MinivanCar`, `ElectricCar`) and tariff plans (`PerMinuteTariff`, `DailyTariff`, `PackageTariff`). Features include:
- Age and license validation before rental
- Dynamic cost calculation with night surcharge multipliers
- Bonus points earning and spending for discounts
- Conflict detection for overlapping bookings
- Rental history tracking per vehicle and customer
### 📚 Online Courses
A full-featured e-learning platform with `Course` → `Module` → `Lesson` hierarchy. Lesson types include:
- `VideoLecture` - pre-recorded content
- `Webinar` - live online sessions
- `TextMaterial` - reading assignments
- `TestAssignment` - auto-graded quizzes
- `TeacherCheckedAssignment` - manually reviewed tasks
Each lesson type carries a different progress weight. Students can enroll in courses, complete lessons sequentially, leave reviews, and earn certificates upon full completion. All progress is persisted between sessions.
### 🚇 Moscow Metro
A graph-based metro route planner with full support for transfers between lines. Features include:
- 5 lines (Sokolnicheskaya, Zamoskvoretskaya, Arbatsko-Pokrovskaya, Filevskaya, Koltsevaya)
- 20+ stations with cross-line transfer connections
- Dijkstra's algorithm for shortest-path routing
- Peak/off-peak intervals affecting wait times
- Emergency situations: line delays and station closures
- Time-of-day aware schedule display
### 🏪 Vending Machine
A retail automation system managing `ColdDrink`, `HotDrink`, and `Snack` products. Core capabilities:
- Expiration date tracking with automatic product removal
- Stock management and replenishment workflows
- Coin and banknote payment (1₽ to 1000₽ denominations)
- Exact change calculation with breakdown by denomination
- Maintenance mode toggle
- Sales statistics with per-product revenue breakdown
### ✈️ Airport
A comprehensive airport operations system handling flights, passengers, tickets, and luggage. Supports:
- `DomesticFlight` and `InternationalFlight` with visa requirements
- Passenger tiers: `EconomyPassenger`, `BusinessPassenger`, `VIPPassenger` (each with different baggage allowances and priority levels)
- Ticket classes: `EconomyTicket`, `BusinessTicket`, `FirstClassTicket`
- Luggage types: `CarryOnLuggage` and `CheckedLuggage` with overweight fee calculation
- Online and airport check-in
- Priority-based boarding with automatic sorting
- Flight delays, cancellations, and connection time validation
- Per-flight statistics dashboard
## 🎯 Key Features Across All Subsystems
- **Robust input validation** with retry loops on invalid data
- **Custom exception hierarchies** for domain-specific error handling
- **Polymorphic class hierarchies** with virtual methods and proper cleanup
- **Persistent storage** with custom serialization (`toJson` / `fromJson`)
- **State management** (flight status, luggage location, emergency situations)
- **Console UI** with colored output and structured menus
- **Task-repeat workflow** - each operation can be executed again without re-entering the subsystem
## 📄 License
The project is intended for educational purposes. It can be used and modified freely.