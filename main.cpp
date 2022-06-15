#include <iostream>
#include "shop.h"


int main()
{
	
	Order order;
	order.get_info();

	PriceBase& price = PriceBase::Instance();
	WeightPosition pp1(std::make_unique<WeightProduct>(price.get_product_weight_price("Apple")), 0.5);
	order.add_position(std::make_unique<WeightPosition>(std::move(pp1)));
	order.get_info();

	AmountPosition pp2(std::make_unique<AmountProduct>(price.get_product_amount_price("Pensile")), 2);
	order.add_position(std::make_unique<AmountPosition>(std::move(pp2)));
	order.get_info();

	WeightPosition pp3(std::make_unique<WeightProduct>(price.get_product_weight_price("Apple")), 1.5);
	order.add_position(std::make_unique<WeightPosition>(std::move(pp3)));
	order.get_info();

	Client Ivan;
	Ivan.pay_and_recieve_order(order);

	Ivan.earn_money(500);
	Ivan.pay_and_recieve_order(order);

	system("pause");
	return 0;
}
