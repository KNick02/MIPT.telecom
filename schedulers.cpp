/*
 * schedulers.cpp
 *
 *  Created on: 9 мар. 2022 г.
 *      Author: Nikolay Kozlovsky
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

const double slot_len 		= 0.07;
const double frm_len 		= 1.0 / 50;
const double almost_zero 	= 0.001;
const double exec_time 		= 2;

struct User
{
	User(double rate, size_t nFrames, bool exp_frames = false) : speed_rate(rate), frm_arrival(nFrames)
	{
		if (exp_frames)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::exponential_distribution<> distr(1 / frm_len);

			for (size_t n = 0; n < nFrames; n++)
				frm_arrival[n] = (n == 0) ? 0 : (frm_arrival[n - 1] + distr(gen));
		}

		else
		{
			for (size_t n = 0; n < nFrames; n++)
				frm_arrival[n] = n * (exec_time / nFrames);
		}
	}

	void UpdateFrames(size_t slot)
	{
		auto last_received = std::upper_bound(frm_arrival.begin(), frm_arrival.end(), slot * slot_len);
		not_sent = (last_received - frm_arrival.begin()) - sent;
	}

	void TakeSlot()
	{
		if (not_sent == 0)
			return;

		not_sent--;
		sent++;
	}

	double speed_rate;
	unsigned not_sent = 0, sent = 0;
	std::vector<double> frm_arrival;
	bool exp_frames = false;

};


double MaxRateDelta(User& user, size_t slot)
{
	user.UpdateFrames(slot);

	if (user.not_sent == 0)
		return 0;

	return user.speed_rate;
}


double MaxMinRateDelta(User& user, size_t slot)
{
	user.UpdateFrames(slot);

	if (user.not_sent == 0)
		return 0;

	return 1 / ((user.sent == 0) ? almost_zero : (1.0 * user.sent / slot));
}


double PFDelta(User& user, size_t slot)
{
	user.UpdateFrames(slot);

	if (user.not_sent == 0)
		return 0;

	return user.speed_rate / ((user.sent == 0) ? almost_zero : (1.0 * user.sent / slot));
}


int main()
{
	size_t nSlots = exec_time / slot_len;
	size_t nFrames = exec_time / frm_len;
	double total_capacity = 0;

	std::vector<User> users = {{72, nFrames, false}, {54, nFrames, false}, {36, nFrames, false}};
	std::vector<double> current_rate(users.size());

	for (size_t i = 0; i < nSlots; i++)
	{
		for (size_t j = 0; j < users.size(); j++)
			current_rate[j] = PFDelta(users[j], i);

		size_t best_fit = std::max_element(current_rate.begin(), current_rate.end()) - current_rate.begin();
		users[best_fit].TakeSlot();
	}

	for (size_t j = 0; j < users.size(); j++)
	{
		std::cout << "user" << j << ' ' << (1.0 * users[j].sent / nSlots) << std::endl;
		total_capacity += (1.0 * users[j].sent / nSlots) * users[j].speed_rate;
	}

	std::cout << "total capacity " << total_capacity << std::endl;

	return 0;
}
