#pragma once
#include <string.h>
#include <sstream>
#include <vector>
#include <map>

class ProductInfo
{
public:
	ProductInfo(std::string info):m_info(info) {}
	std::string get_info() { return m_info; }
private:
	std::string m_info;
};

class Product
{
public:
	Product(ProductInfo& product_info):m_product_info(product_info) {}
	virtual std::string get_info() = 0;
	virtual double get_cost() = 0;
protected:
	ProductInfo& m_product_info;
};

class WeightProduct: public Product
{
public:
	WeightProduct(ProductInfo& product_info, double cost):m_cost_per_kg(cost), Product(product_info) {}
	double get_cost() {return m_cost_per_kg;}
	std::string get_info() {
		std::ostringstream out(std::ostringstream::ate);
		out << m_product_info.get_info() << " : " << m_cost_per_kg << " per kg" << std::endl;
		return out.str();
	}
private:
	double m_cost_per_kg;
};

class AmountProduct : public Product
{
public:
	AmountProduct(ProductInfo& product_info, double cost) :m_cost_per_one(cost), Product(product_info) {}
	double get_cost() { return m_cost_per_one; }
	std::string get_info() {
		std::ostringstream out(std::ostringstream::ate);
		out << m_product_info.get_info() << " : " << m_cost_per_one << " per one"<<std::endl;
		return out.str();
	}
private:
	double m_cost_per_one;
};

class Position
{
public:
	Position(std::unique_ptr<Product> product):m_ptr_product(std::move(product)) {}
	std::unique_ptr<Product>& get_ptr_product() { return m_ptr_product; }
	virtual double get_cost() = 0;
	virtual double get_quanity() = 0;
protected:
	std::unique_ptr<Product> m_ptr_product;
};

class AmountPosition: public Position
{
public:
	AmountPosition(std::unique_ptr<AmountProduct> product, size_t amount):Position(std::move(product)), m_amount(amount) {}
	double get_cost() { return (m_amount* (m_ptr_product->get_cost())); }
	double get_quanity() { return m_amount; }
private:
	size_t m_amount;
};

class WeightPosition : public Position
{
public:
	WeightPosition(std::unique_ptr<WeightProduct> product, double weight):Position(std::move(product)), m_weight(weight) {}
	double get_cost() override { return (m_weight * (m_ptr_product->get_cost())); }
	double get_quanity() override { return m_weight; }
private:
	double m_weight;
};

class Order
{
public:
	Order() = default;
	
	void add_position(std::unique_ptr<Position> ptr_pos) {
		auto pos = find_if(m_ptr_positions.begin(), m_ptr_positions.end(), 
			[&ptr_pos](std::unique_ptr<Position>& pos) {return pos->get_ptr_product()->get_info() == ptr_pos->get_ptr_product()->get_info();});
		if (pos != m_ptr_positions.end())
			swap(*pos, ptr_pos);
		else
			m_ptr_positions.push_back(std::move(ptr_pos));
	}

	double get_cost() {
		double cost = 0;
		for (auto& pos : m_ptr_positions)
			cost += pos->get_cost();
		return cost;
	}

	void get_info() {
		for (auto& pos : m_ptr_positions)
		{
			std::cout << pos->get_ptr_product()->get_info() <<"\t" << "Quanity : " << pos->get_quanity()
				<< std::endl << "\t" << "Cost : " << pos->get_cost() << std::endl;
		}
		std::cout << "Total cost: " << get_cost() << std::endl;
		std::cout << "----------------------------------------------------" << std::endl;
	}

	bool empty() { return !m_ptr_positions.size(); }
private:
	std::vector<std::unique_ptr<Position>> m_ptr_positions;
	friend class Client;
	void clear() { m_ptr_positions.clear(); }
};

class Client
{
public:
	void earn_money(double money) { m_balance.add_money(money); }

	bool pay_and_recieve_order(Order& order) {
		if (m_balance.waste_money(order.get_cost()))
		{
			order.clear();
			std::cout << "The order is recieved" << std::endl;
			return true;
		}
		else
		{
			std::cout << "Not enough money to pay for order!" << std::endl;
			return false;
		}
	}

	class Balance
	{
	public:
		void add_money(double money) { m_money += money; }
		bool waste_money(double money) {
			if (m_money - money > std::numeric_limits<double>::epsilon())
			{
				m_money -= money;
				return true;
			}
			else
				return false;				
		}
	private:
		double m_money = 0;
	};

private:
	Balance m_balance;
};

class PriceBase
{
public:
	PriceBase(const PriceBase&) = delete;
	PriceBase& operator = (const PriceBase&) = delete;
	static PriceBase& Instance() { // Использует отложенную инициализацию
		static PriceBase instance; // тут сработает дефолтный конструктор
		return instance;
	}
	WeightProduct get_product_weight_price(std::string name) { return m_products_weight_price.at(name); }
	AmountProduct get_product_amount_price(std::string name) { return m_products_amount_price.at(name); }

protected:
	PriceBase() {
		m_products_info =
		{
		 { "Apple", ProductInfo("Green sweet apple") },
		 { "Nuts", ProductInfo("Macadamia nut") },
		 { "Pensile", ProductInfo("Red pensile with rubber") },
		 { "Chair", ProductInfo("Wooden chair with armrests") },
		};

		m_products_weight_price =
		{
		 { "Apple", WeightProduct(m_products_info.at("Apple"), 11.3) },
		 { "Nuts", WeightProduct(m_products_info.at("Nuts"), 34.2) },
		};

		m_products_amount_price =
		{
		 { "Pensile", AmountProduct(m_products_info.at("Pensile"), 5.7) },
		 { "Chair", AmountProduct(m_products_info.at("Chair"), 75) },
		};
	}
private:
	std::map<std::string, ProductInfo> m_products_info;
	std::map<std::string, WeightProduct> m_products_weight_price;
	std::map<std::string, AmountProduct> m_products_amount_price;
};