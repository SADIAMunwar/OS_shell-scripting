**Simulation of Banking Transaction Processing System**

Objective:
To design and implement a simulated banking transaction processing system using operating system
concepts. This project will focus on managing financial resources, coordinating transactions across
multiple accounts and branches, and handling synchronization in a banking environment with
concurrent operations and security requirements.

Scenario:
You are tasked with simulating a modern banking system where multiple customers perform various
financial transactions simultaneously across different branches and ATMs. The banking system has
limited resources such as available funds, ATM machines, teller windows, and security protocols.
Banking operations involve various components like transaction processors, account managers,
security validators, and audit systems. Transaction data and authorization commands are distributed
through a central banking management system which acts as a coordination hub. At any given time,
only limited numbers of transactions can be processed simultaneously for security and system stability,
so transactions must wait for appropriate authorization and resource availability before execution. If
system overload occurs at one branch or ATM network, other connected banking services cannot
operate at full capacity until the load is balanced. Your goal is to create an operating system-based
simulation that efficiently manages banking operations and coordinates financial services.

Tasks:
You have to do the following tasks for the simulation:
1. Resource Management:

• Simulate the availability of banking resources such as available funds, ATM machines,
teller windows, security tokens, and dynamically generated transaction processing
capacity.
• Implement resource allocation mechanisms to ensure controlled access by customers
across different branches and service channels.

• Introduce resource replenishment based on fund transfers, daily limits reset, or time-
based banking operation cycles.

2. Process and Thread Management:

• Design concurrent execution models to handle multiple banking operations
simultaneously.
• Simulate the creation and termination of execution units based on customer requests,
account availability, and security clearance.
• Utilize synchronization mechanisms for coordinated access to shared banking resources
and account information.

3. Synchronization:

• Develop synchronization mechanisms to manage access to shared resources like
customer accounts, transaction logs, and security systems.
• Ensure that simultaneous access to critical financial data does not lead to conflicts and
implement synchronization to maintain transaction integrity and prevent race
conditions.
• Employ mutex, semaphores, and condition variables for banking operation
synchronization.
4. Memory Management:
• Implement memory management to store data related to banking operations, including
account balances, transaction histories, security logs, processing queues, and dynamic
adjustments based on transaction priorities.
• Optimize memory usage for quick account data retrieval and real-time transaction
updates.
• Utilize shared memory for inter-component communication between different banking
service modules.
5. Priority Scheduling:

• Implement a priority scheduling algorithm to efficiently handle critical banking
operations and high-value transactions.
• Define criteria for transaction prioritization, such as VIP customers, large transfers,
time-sensitive payments, or security alerts.
• Adapt dynamically to changes in transaction priorities based on real-time banking
conditions and customer status.

Step 5.1 Assigning Priorities:

• Each banking transaction is associated with a priority level based on importance and
urgency. Higher-priority transactions are assigned lower priority numbers.
• Priority levels are defined based on the criticality and impact on banking operations and
customer service.

Step 5.2 Example Priority Levels:
• Critical Priority: Security alerts, fraud detection, system maintenance, regulatory
compliance.
• High Priority: VIP customer transactions, large value transfers, time-critical payments.
• Medium Priority: Regular customer transactions, standard transfers, account inquiries.
• Low Priority: Balance checks, transaction history requests, non-urgent account updates.
Step 5.3 Assignment of Priority Levels:
• Transactions are categorized into different priority levels based on customer type,
transaction amount, and current banking policies.
• Each transaction is tagged with its priority level, reflecting its importance to banking
operations.
Step 5.4 Scheduling Logic:
• The priority scheduling algorithm processes the highest priority transactions first.
• If two transactions have the same priority, other criteria such as submission time or
customer relationship status may be employed as tiebreakers.

Step 5.5 Maintain Separate Transaction Queues:
• The system maintains separate transaction queues for each priority level (critical, high,
medium, low).
• Each queue contains transactions assigned to its respective priority level based on banking
service policies.
Step 5.6 Selecting Transactions:
• The system processes transactions from the highest priority queue first.
• If the critical priority queue is empty, transactions from the high priority queue are
considered, and so on.
• The selected transactions are then assigned to available banking resources and personnel
for processing.

Step 5.7 Dynamic Priority Adjustments:

• The priority scheduling should be dynamic, allowing the system to adapt to changes in
banking conditions based on real-time monitoring or security alerts.
• If a security incident occurs or unusual transaction patterns are detected, the priority of
related operations can be dynamically adjusted.

Step 5.8 Integration with I/O Simulation:

• Integrate the priority scheduling algorithm with an I/O simulation that mimics system
maintenance, network issues, or security incidents.
• During incidents, certain transaction types may need to be suspended or require additional
verification, and the scheduling algorithm must adjust priorities accordingly.

Step 5.9 Consideration of System Capacity:

• When processing transactions, the system considers the current load and capacity of
different banking infrastructure components.
• Ensure that infrastructure with sufficient capacity is available for high-priority
transactions to optimize banking service delivery.

Step 5.10 Efficient Resource Utilization:

• Optimize resource utilization by directing critical transactions through the most
appropriate and available processing channels.

Step 5.11 Dynamic Changes in Priority:

• Allow for dynamic adjustments in transaction priorities based on real-time banking
system monitoring.
• For instance, if fraud is detected or a security breach occurs, related account operations
should be escalated to highest priority, and the scheduling algorithm should adapt
accordingly.
6. I/O Management:

• Integrate I/O operations to simulate external factors affecting banking operations, such as
network connectivity issues, system maintenance windows, regulatory updates, or
security incidents.
• Simulate delays in transaction processing due to system issues or security checks and
coordinate banking operations accordingly.

7. Error Handling:

• Implement robust error-handling mechanisms for unexpected scenarios, such as network
failures, insufficient funds, account locks, fraud detection, and system overloads.
• Ensure that the system can recover gracefully from errors without compromising financial
data integrity or customer service.

8. Dynamic Resource Generation:
• Simulate the dynamic generation and adjustment of banking processing capacity over time.
• Implement mechanisms for adaptive transaction limits, temporary resource allocation, and
system scaling based on customer demand and banking requirements.

9. Dynamic Task Assignment:
• Develop algorithms for dynamically assigning banking operations based on current system
load, security requirements, and customer service policies.
• Consider factors like transaction complexity, security clearance levels, and adjust
processing strategies accordingly.

10. Realistic System Behavior:
• Simulate realistic banking system behavior, including business hours, maintenance
periods, peak transaction times, and security protocols.
• If a banking service component needs maintenance or experiences issues, it is removed
from the active processing queue. For component replacement and load balancing
optimization, implement the LRU (Least Recently Used) technique for resource
management.
