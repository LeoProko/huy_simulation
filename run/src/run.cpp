#include "run.h"

void Run::init() {
    std::cout << "INITIALIZATION BEGIN\n";
    progress = 0.;
    progress_scale = 1. / parameters::days_amount;
    map_ = Map();
    for (int i = 0; i < parameters::bots_amount; ++i) {
        all_bots.emplace_back();
    }
    output_file_ = File("visualization/results.json");
    std::cout << "INITIALIZATION COMPLETED\n";
}

void Run::print_average() const {
    int avg_militancy = 0;
    int avg_intelligence = 0;
    int avg_children_amount = 0;
    int avg_children_health = 0;
    int avg_vision = 0;
    int avg_health = 0;
    int avg_lifetime = 0;
    int altruist_amount = 0;
    int greenbeared_amount = 0;
    int greenbeared_altruists_amount = 0;
    for (auto& bot : all_bots) {
        avg_militancy += bot.militancy_;
        avg_intelligence += bot.intelligence_;
        avg_children_amount += bot.children_amount_;
        avg_children_health += bot.children_health_;
        avg_vision += bot.vision_;
        avg_health += bot.health_;
        avg_lifetime += bot.lifetime_;
        altruist_amount += bot.is_altruist_;
        greenbeared_amount += bot.is_greenbeared_;
        greenbeared_altruists_amount += bot.is_altruist_ && bot.is_greenbeared_;
    }
    avg_militancy /= all_bots.size();
    avg_intelligence /= all_bots.size();
    avg_children_amount /= all_bots.size() * 10;
    avg_children_health /= all_bots.size();
    avg_vision = (avg_vision / all_bots.size()) / 10;
    avg_health /= all_bots.size();
    avg_lifetime /= all_bots.size();
    std::cout << "Average militancy............." << avg_militancy << "\n";
    std::cout << "Average intelligence.........." << avg_intelligence << "\n";
    std::cout << "Average vision................" << avg_vision << " \n";
    std::cout << "Average children amount......." << avg_children_amount << "\n";
    std::cout << "Average children health......." << avg_children_health << "\n";
    std::cout << "Average health................" << avg_health << "\n";
    std::cout << "Average lifetime.............." << avg_lifetime << "\n";
    std::cout << "Altruists amount.............." << altruist_amount << "\n";
    std::cout << "Greenbeared amount............" << greenbeared_amount << "\n";
    std::cout << "Greenbeared altruists amount.." << greenbeared_altruists_amount << "\n";
}

void Run::print_progress(int today) {
    if (progress - 0. >= 1e-6) {
        for (int i = 0; i < 13; ++i) {
            std::cout << "\033[F\x1b[2K";
            std::cout.flush();
        }
    }
    progress += progress_scale;
    std::cout << "[";
    int position = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < position) {
            std::cout << "=";
        }
        else if (i == position) {
            std::cout << ">";
        }
        else {
            std::cout << " ";
        }
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << "%\n";
    std::cout << "Day number " << today << "\n";
    std::cout << "Bots amount: " << all_bots.size() << " \n";
    print_average();
}

void Run::run() {
    std::cout << "START SIMULATION WITH PARAMETERS:\n";
    parameters::print();
    init();
    for (int today = 0; today <= parameters::days_amount; ++today) {
        print_progress(today);
        for (auto bot_iter = all_bots.begin(); bot_iter != all_bots.end(); ++bot_iter) {
            if (bot_iter->health_ > parameters::damage) {
                move(*bot_iter, map_);
            }
        }
        std::list<Bot> new_bots;
        for (auto& bot : all_bots) {
            map_[bot.position_].do_all(new_bots);
        }
        nlohmann::json json_result = map_;
        output_file_.print(json_result);
        for (auto& bot : all_bots) {
            map_[bot.position_].clean();
        }
        for (auto bot_iter = all_bots.begin(); bot_iter != all_bots.end();) {
            if (bot_iter->health_ <= parameters::damage) {
                auto bot_iter_to_erase = bot_iter++;
                all_bots.erase(bot_iter_to_erase);
            } else {
                ++bot_iter;
            }
        }
        for (const auto& new_bot : new_bots) {
            all_bots.push_back(new_bot);
        }
        map_.clean_and_respawn();
        if (all_bots.size() == 0) {
            break;
        }
    }
    if (all_bots.size() == 0) {
        std::cout << "SIMULATION FAILED. ALL BOTS DEAD!\n";
    } else {
        std::cout << "SUCCESSFUL SIMULATION. CONGRATULAIONS!\n";
    }
}
