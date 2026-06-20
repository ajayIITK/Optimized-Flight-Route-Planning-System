
#include <bits/stdc++.h>
using namespace std;
struct Flight
{
    int flight_no;
    int start_city;
    int departure_time;
    int end_city;
    int arrival_time;
    int fare;
    Flight(int fn, int sc, int dt, int ec, int at, int f)
    {
        flight_no = fn;
        start_city = sc;
        departure_time = dt;
        end_city = ec;
        arrival_time = at;
        fare = f;
    }
};
struct Node
{
    Flight flight;
    Node *parent;
    int depth;

    Node(Flight f, Node *p, int d) : flight(f), parent(p), depth(d) {}
};

class Planner
{

private:
    vector<Flight> flights;
    int no_flights;
    int m; // no of cities
    vector<vector<Flight>> all_flights;
    // all_flights[i] stores all flights that start from city i
    vector<vector<Flight>> possible_flights;
    // possible_flights[i] stores all flights that can be taken after flight i

public:
    Planner(const vector<Flight> &input_flights)
    {
        flights = input_flights;
        no_flights = flights.size();
        m = 0;
        for (const auto &f : flights)
        {
            m = max({m, f.start_city, f.end_city});
        }
        all_flights.resize(m + 1);
        for (const auto &f : flights)
        {
            all_flights[f.start_city].push_back(f);
        }
        possible_flights.resize(no_flights);
        for (const auto &f1 : flights)
        {
            for (const auto &f2 : all_flights[f1.end_city])
            {
                if (f2.departure_time - f1.arrival_time >= 20)
                {
                    possible_flights[f1.flight_no].push_back(f2);
                }
            }
        }
    }
    vector<Flight> least_flights_earliest_route(int start_city, int end_city, int t1, int t2)
    {
        if (start_city == end_city)
            return {};
        // using "parent" information to backtrack and reconstruct the route taken
        queue<Node *> q;
        vector<Flight> best_path;
        int best_arrival_time = INT_MAX;
        int max_depth = INT_MAX;                  // Track minimum number of flights used
        vector<int> visited(no_flights, INT_MAX); // Track earliest arrival time for each flight
        for (const Flight &f : all_flights[start_city])
        {
            if (f.departure_time >= t1 && f.departure_time <= t2)
            {
                visited[f.flight_no] = f.arrival_time;
                q.push({new Node(f, nullptr, 0)}); // depth is number of flights used
            }
        }
        while (!q.empty())
        {
            Node *current = q.front();
            q.pop();
            Flight flight = current->flight;
            int depth = current->depth;
            if (depth > max_depth)
            {
                delete current;
                break;
            }
            // If we reach end_city within valid time
            if (flight.end_city == end_city && flight.arrival_time <= t2)
            {
                // Better route: fewer flights or earlier arrival
                if (flight.arrival_time < best_arrival_time || depth < max_depth)
                {
                    best_arrival_time = flight.arrival_time;
                    max_depth = depth;
                }
                // Backtrack to reconstruct path
                best_path.clear();
                Node *temp = current;
                while (temp != nullptr)
                {
                    best_path.push_back(temp->flight);
                    temp = temp->parent;
                } // best_path flightlast,parent,parent_parent ...
                reverse(best_path.begin(), best_path.end());
                // best_path is now correct
                for (const Flight &next : possible_flights[flight.flight_no])
                {
                    if (next.arrival_time <= t2 && visited[next.flight_no] > next.arrival_time)
                    {
                        // store the earlier time
                        visited[next.flight_no] = next.arrival_time;
                        q.push(new Node(next, current, depth + 1));
                    }
                }
            }
        }
        return best_path;
    }
    struct RouteNode
    {
        int cost;
        Flight flight;
        RouteNode *parent;
        RouteNode(int c, Flight f, RouteNode *p) : cost(c), flight(f), parent(p) {}
    };
    struct CompareRoute
    {
        bool operator()(RouteNode *a, RouteNode *b)
        {
            return a->cost > b->cost;
        } // lowest cost has higher priority
    };
    vector<Flight> cheapest_route(int start_city, int end_city, int t1, int t2)
    {
        if (start_city == end_city)
            return {};
        vector<bool> visited(no_flights, false);
        priority_queue<RouteNode*, vector<RouteNode*>, CompareRoute> pq;

        for(const Flight& f : all_flights[start_city]){
              if (f.departure_time >= t1 && f.arrival_time <= t2) {
            pq.push(new RouteNode(f.fare, f, nullptr));
        }
        }
        while(!pq.empty()){
            RouteNode* current = pq.top(); pq.pop();
            int cost = current->cost;
            Flight flight = current->flight;
            if (visited[flight.flight_no] && current->parent != nullptr) { // condition mein doubt
            delete current;
            continue;
        }
        // If destination reached
        if (flight.end_city == end_city) {
            vector<Flight> path;
            RouteNode* trace = current;
            while (trace != nullptr) {
                path.push_back(trace->flight);
                trace = trace->parent;
            }
            reverse(path.begin(), path.end());

            // Free memory
            while (!pq.empty()) {
                delete pq.top();
                pq.pop();
            }

            return path;
        }   
        for (const Flight& next : possible_flights[flight.flight_no]) {
            if (next.arrival_time <= t2 && !visited[next.flight_no]) {
                pq.push(new RouteNode(cost + next.fare, next, current));
            }
        }

        visited[flight.flight_no] = true;  // Mark current flight as processed

        }
        return {};
    }
    struct FlightState {
    Flight flight;               // Current flight
    FlightState* parent;         // Pointer to previous flight in the route
    int depth;                   // Number of flights taken
    int cost;                    // Total fare from start

    FlightState(Flight f, FlightState* p, int d, int c)
        : flight(f), parent(p), depth(d), cost(c) {}
};

vector<Flight> least_flights_cheapest_route(int start_city, int end_city, int t1, int t2) {
    if (start_city == end_city) return {};

    vector<int> visited(no_flights, INT_MAX);
    queue<FlightState*> q;

    int best_cost = INT_MAX, max_depth = INT_MAX;
    vector<Flight> best_path;

    for (auto& f : all_flights[start_city]) {
        if (f.departure_time >= t1 && f.arrival_time <= t2) {
            visited[f.flight_no] = f.fare;
            q.push(new FlightState(f, nullptr, 0, f.fare));
        }
    }

    while (!q.empty()) {
        FlightState* current = q.front(); q.pop();

        if (current->depth > max_depth) continue;

        if (current->flight.end_city == end_city && current->flight.arrival_time <= t2) {
            if (current->cost < best_cost || current->depth < max_depth) {
                best_cost = current->cost;
                max_depth = current->depth;

                best_path.clear();
                FlightState* temp = current;
                while (temp != nullptr) {
                    best_path.push_back(temp->flight);
                    temp = temp->parent;
                }
            }
        }

        for (auto& next : possible_flights[current->flight.flight_no]) {
            int new_cost = current->cost + next.fare;
            if (next.arrival_time <= t2 && visited[next.flight_no] > new_cost) {
                visited[next.flight_no] = new_cost;
                q.push(new FlightState(next, current, current->depth + 1, new_cost));
            }
        }
    }

    reverse(best_path.begin(), best_path.end());
    return best_path;
}
};
