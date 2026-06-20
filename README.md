# Flight Route Planner

A C++ flight itinerary planner that finds routes between cities using three different optimization strategies: fewest flights, cheapest fare, and fewest flights with cheapest fare as a tiebreaker.

## Overview

The planner models a network of flights between cities and searches for valid multi-leg routes within a given departure/arrival time window. A layover of at least **20 minutes** between connecting flights is enforced.

Each flight is represented as:

```cpp
struct Flight {
    int flight_no;
    int start_city;
    int departure_time;
    int end_city;
    int arrival_time;
    int fare;
};
```

## How it works

The `Planner` class is constructed with a list of all available `Flight` objects and precomputes two lookup structures:

- **`all_flights[city]`** — every flight departing from a given city.
- **`possible_flights[flight_no]`** — every flight that can legally be taken immediately after a given flight (same connecting city, and at least 20 minutes of layover).

This precomputation lets each search algorithm explore connections in O(1) per edge instead of rescanning all flights.

### Search algorithms

| Method | Goal | Approach |
|---|---|---|
| `least_flights_earliest_route` | Minimum number of legs, then earliest arrival | BFS over flights, tracking depth and arrival time |
| `cheapest_route` | Minimum total fare | Dijkstra-style search using a min-priority-queue keyed on cumulative fare |
| `least_flights_cheapest_route` | Minimum number of legs, then minimum fare | BFS over flights, tracking depth and cumulative cost |

All three methods take `(start_city, end_city, t1, t2)` — the earliest allowed departure and the latest allowed arrival — and return the route as an ordered `vector<Flight>`, or an empty vector if no valid route exists.

Routes are reconstructed by walking parent pointers from the destination node back to the start, then reversing the resulting path.

## Building

This is a single header-style file using only the C++ standard library (`bits/stdc++.h`). It has no external dependencies.

```bash
g++ -std=c++17 -O2 -o planner main.cpp
```



## Example usage

```cpp
vector<Flight> flights = {
    Flight(1, 0, 100, 1, 200, 50),
    Flight(2, 1, 230, 2, 330, 60),
    Flight(3, 0, 100, 2, 400, 150),
};

Planner planner(flights);

auto route = planner.cheapest_route(0, 2, 0, 1000);
for (const auto& f : route) {
    cout << "Flight " << f.flight_no << ": " << f.start_city
         << " -> " << f.end_city << " ($" << f.fare << ")\n";
}
```

