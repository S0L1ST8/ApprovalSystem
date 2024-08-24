#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>

class Role {
  public:
    virtual double ApprovalLimit() const noexcept = 0;
    virtual ~Role() = default;
};

class EmployeeRole : public Role {
  public:
    double ApprovalLimit() const noexcept override {
        return 1000;
    }
};

class TeamManagerRole : public Role {
    double ApprovalLimit() const noexcept override {
        return 10000;
    }
};

class DepartmentManagerRole : public Role {
    double ApprovalLimit() const noexcept override {
        return 100000;
    }
};

class PresidentRole : public Role {
    double ApprovalLimit() const noexcept override {
        return std::numeric_limits<double>::max();
    }
};

struct Expense {
    double amount;
    std::string description;

    Expense(double amount, std::string_view desc) : amount(amount), description(desc) {}
};

class Employee {
  public:
    explicit Employee(std::string_view name, std::unique_ptr<Role> own_role) : name_(name)
                                                                             , own_role_(std::move(own_role)) {}

    void SetDirectManager(std::shared_ptr<Employee> manager) {
        direct_manager_ = manager;
    }

    void Approve(const Expense& e) {
        if (e.amount <= own_role_->ApprovalLimit()) {
            std::cout << name_ << " approved expense '" << e.description
                      << "', cost=" << e.amount << std::endl;
        }
        else if (direct_manager_) {
            direct_manager_->Approve(e);
        }
    }

  private:
    std::string name_;
    std::unique_ptr<Role> own_role_;
    std::shared_ptr<Employee> direct_manager_;
};

int main() {
    auto john = std::make_shared<Employee>("john smith", std::make_unique<EmployeeRole>());
    auto robert = std::make_shared<Employee>("robert booth", std::make_unique<TeamManagerRole>());
    auto david = std::make_shared<Employee>("david jones", std::make_unique<DepartmentManagerRole>());
    auto cecil = std::make_shared<Employee>("cecil williamson", std::make_unique<PresidentRole>());

    john->SetDirectManager(robert);
    robert->SetDirectManager(david);
    david->SetDirectManager(cecil);

   john->Approve(Expense{500, "magazins"});
   john->Approve(Expense{5000, "hotel accomodation"});
   john->Approve(Expense{50000, "conference costs"});
   john->Approve(Expense{200000, "new lorry"});
}
