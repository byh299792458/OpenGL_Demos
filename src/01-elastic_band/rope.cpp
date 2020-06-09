#include "rope.hpp"

using namespace std;

Rope::Rope(glm::vec3 start, glm::vec3 end, int num_nodes, float node_mass, float k,
    vector<int> pinned_nodes)
{
    auto delta = (end - start) / glm::vec3(num_nodes - 1);
    for (int i = 0; i < num_nodes; ++i) {
        masses_.push_back(new Mass(start + delta * glm::vec3(i), node_mass, false));
    }
    for (int i = 0; i < num_nodes - 1; ++i) {
        springs_.push_back(new Spring(masses_[i], masses_[i + 1], k));
    }
    for (auto&& i : pinned_nodes)
    {
        masses_[i]->is_pinned_ = true;
    }
}

void Rope::SimulateVerlet(double delta_t, glm::vec3 gravity, double kd)
{
    for (auto&& s : springs_)
    {
        auto dist = glm::distance(s->m1_->position_, s->m2_->position_);
        auto vec12 = s->m1_->position_ - s->m2_->position_;
        auto f12 = -(vec12 * glm::vec3(s->k_ * (dist - s->rest_length_) / dist));

        s->m1_->forces_ += f12;
        s->m2_->forces_ -= f12;
    }

    for (auto&& m : masses_)
    {
        if (!m->is_pinned_)
        {
            glm::vec3 temp_position = m->position_;
            m->forces_ += gravity;
            auto a = m->forces_ / glm::vec3(m->mass_);
            m->position_ = m->position_
                + glm::vec3(1 - 0.00005) * (m->position_ - m->last_position_)
                + a * glm::vec3(delta_t) * glm::vec3(delta_t);
            m->last_position_ = temp_position;
        }
        m->forces_ = glm::vec3(0, 0, 0);
    }
}

void Rope::SimulateEuler(double delta_t, glm::vec3 gravity, double kd)
{
    for (auto&& s : springs_) 
    {
        auto dist = glm::distance(s->m1_->position_, s->m2_->position_);
        auto vec12 = s->m1_->position_ - s->m2_->position_;
        auto f12 = -(vec12 * glm::vec3(s->k_ * (dist - s->rest_length_) / dist));

        s->m1_->forces_ += f12;
        s->m2_->forces_ -= f12;
    }

    for (auto&& m : masses_)
    {
        if (!m->is_pinned_) 
        {
            m->forces_ += gravity;
            m->forces_ -= glm::vec3(0.005) * m->velocity_;
            auto a = m->forces_ / glm::vec3(m->mass_);
            //m->position_ += m->velocity_ * delta_t;
            m->velocity_ += a * glm::vec3(delta_t);
            m->position_ += m->velocity_ * glm::vec3(delta_t);
        }
        m->forces_ = glm::vec3(0, 0, 0);
    }
}
