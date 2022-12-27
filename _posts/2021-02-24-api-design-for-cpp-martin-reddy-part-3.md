---
layout: single
title: "A summary of API Design for C++ - Martin Reddy (part 3)"
date: 2021-02-24T20:47:00+07:00
categories: book
classes:
  - dark-theme
tags:
  - technology
  - api
  - c++
  - design patterns
---

How to make a high-quality API design

{% include toc icon="cog" title="Table of Contents" %}

# Design

## A case for good design

### Accuring technical debt
+ Technical debt accrue over time with the bad design

Indicators of accruing technical debt:
* Fragility
* Rigidity
* Immobility
* Non-traserability

#### Paying back the debt

Two extremes for the next-generation project:
+ Evolution: modify and adapt to new requirements.
+ Revolution: implement completely new system from the scratch.

### Design for the long term

+ Reasons why design priciples are not adopted and result in technical debts.

## Gathering functional requirements
The first step in producing a good design for a piece of software is to understand what it actually needs to do. The work must be specified in enough detail such that everyone involved could form a shared understanding and work toward the same goal. This is where requirements come in.

Several types of requirements:
* Business requirements
* Functional requirements
* Non-functional requirements

### Functional requirements

> Functional requirements specify how your API should behave.

Example of questions to ask the user about functional requirements:
* What tasks they expect to achieve with the API?
* What an optimal workflow would be from their perspective?
* What are all the potential inputs, including their types and valid ranges?
* What are all the expected outputs, including type, format, and ranges?
* What file formats or protocols must be supported?
* What (if any) mental models do they have for the problem domain?
domain terminology do they use?

Functional requirements can also be supported by non-functional requirements. Some examples of aspects of non-functional requirements:

* Performance
* Platform compatibility
* Security
* Fexibility
* Usability
* Concurrency
* Cost

Good functional requirements should be simple, easy to read, unambiguous, verifiable, and free of jargon. It’s important that they **do not over-specify the technical implementation**: functional requirements should document **what an API should do and not how it does it**.

## Creating use cases

A use case describes the behavior of an API based on interactions of a user or another piece of software (Jacobson, 1992). Use cases are essentially a form of functional requirement that specifically
captures who does what with an API and for what purpose rather than simply providing a list of features, behaviors, or implementation notes. Focusing on use cases helps you design an API from the perspective of the client.

> Use cases can be simple lists of short goal-oriented descriptions or can be more formal structured specifications that follow a prescribed template.

It’s not uncommon to produce a functional requirement document as well as a set of use cases. For example, use cases can be used to describe an API from the user’s point of view, whereas functional requirements can be used to describe a list of features or the details of an algorithm.

However, concentrating on just one of these techniques can often be sufficient, too. In which case, I recommend creating use cases because these resonate most closely with the way a user wants to
interact with a system. When using both methods, you can either derive functional requirements from the use cases or vice versa, although **it’s more typical to work with your users to produce use cases first and then derive a list of functional requirements from these use cases**.

### Developing use cases

A good way to approach the process of creating use cases is therefore (1) identify all of the actors of the system and the roles that each plays, (2) identify all of the goals that each role needs to accomplish, and (3) create use cases for each goal.

> Use cases can be simple lists of short goal-oriented descriptions or can be more formal structured specifications that follow a prescribed template.

Example of a use case template:

* Name: A unique identifier for the use case, often in verb–noun format such as Withdraw Cash or Buy Stamps.
* Version: A number to differentiate different versions of the use case.
* Description: A brief overview that summarizes the use case in one or two sentences.
* Goal: A description of what the user wants to accomplish.
* Actors: The actor roles that want to achieve the goal.
* Stakeholder: The individual or organization that has a vested interest in the outcome of the use case, for example, an ATM User or the Bank.
* Basic Course: A sequence of steps that describe the typical course of events. This should avoid conditional logic where possible.
* Extensions: A list of conditions that cause alternative steps to be taken. This describes what to do if the goal fails, for example, an invalid PIN number was entered.
* Trigger: The event that causes the use case to be initiated.
* Precondition: A list of conditions required for the trigger to execute successfully.
* Postcondition: Describes the state of the system after the successful execution of the use case.
* Notes: Additional information that doesn’t fit well into any other category.

### Writing good use cases

General guidelines and words of advice for writing use case:
* Use domain terminology: The terms that are used should be familiar to users and should come from the domain being targeted.
* Don't over-specify use cases: Use cases should describe the black-box functionality of a system, that is, you should avoid specifying implementation details. You should also avoid including too much detail in your use cases.
* Use cases don't define all requirements.
* Use cases don't define a design.
* Don't specify design in use cases.
* Use cases can direct testing: Use cases are not test plans in themselves because they don’t specify specific input and output values. However, they do specify the key workflows that your users expect to be able to achieve.
* Expect to iterate.
* Don't insist on complete coverage: For the same reasons that use cases do not encompass all forms of requirements, you should not expect your use cases to express all aspects of your API. However,
you also don’t need them to cover everything. Some parts of the system may already be well understood or do not need a user-directed perspective. There’s also the logistical concern that because you will not have unlimited time and resources to compile exhaustive use cases, you should focus the effort on the most important user-oriented goals and workflows.

A sample use case for entering a PIN number in the ATM example:

* Name: Enter PIN
* Version: 1.0.
* Description: User enters PIN number to validate her Bank account information.
* Goal: System validates User’s PIN number.
* Stakeholders:
    1. User wants to use ATM services
    2. Bank wants to validate the User’s account.
* Basic Course:
    1. System validates that ATM card is valid for use with the ATM machine.
    2. System prompts the user to enter PIN number.
    3. User enters PIN number.
    4. System checks that the PIN number is correct.
* Extensions:
    1. System failure to recognize ATM card:

        1.1. System displays error message and aborts operation.
    2. User enters invalid PIN:

        2.1. System displays error message and lets User retry.
* Trigger: User inserts card into ATM.
* Postcondition: User’s PIN number is validated for financial transactions.

### Requirements and Agile development

Agile methodologies therefore deemphasize document-centric processes, instead preferring to iterate on working code. However, this does not mean that they are without any form of requirements. What it means is that the requirements are lightweight and easily changed. Maintaining a large, wordy, formal requirements document would not be considered agile. However, the general concept of use cases is very much a part of agile processes, such as Scrum and XP, which emphasize the creation of user stories.

> User stories are a way to capture minimal requirements from users within an agile development process.

Another important aspect of user stories is that they are written by project stakeholders, not developers, that is, the customers, vendors, business owners, or support personnel interested in the product being developed. Keeping the user story short allows stakeholders to write them in a few minutes.

The user story could be as simple as the format:

As a *[role]* I want *[something]* so that *[benefit]*.

Some examples for user stories in the case of the ATM example:

* As a customer I want to withdraw cash so that I can buy things.
* As a customer I want to transfer money from my savings account to my checking account so I can write checks.
* As a customer I want to deposit money into my account so I can increase my account balance.
* As a bank business owner I want the customer’s identity to be verified securely so that the ATM can protect against fraudulent activities.
* As an ATM operator I want to restock the ATM with money so the ATM will have cash for customers to withdraw.

Cohn also presents an easy-to-remember acronym to help you create good user stories. The acronym is INVEST, where each letter stands for a quality of a well-written user story:

* Independent
* Negotiable
* Valuable
* Estimable
* Small
* Testable

## Elements of API design

The secret to producing a good API design lies in coming up with
an appropriate abstraction for the problem domain and then devising appropriate object and class hierarchies to represent that abstraction.

There are two important hierarchical views of any complex system:
* Object hierarchy: Describes how different objects cooperate in the system.
* Class hierarchy: Describes the common structure and behavior shared between related objects.

Related to this, it is generally agreed that the design phase of software construction consists of two major activities:

1. Architecture design: Describes the top-level structure and organization of a piece of software.
2. Detailed design: Describes individual components of the design to a sufficient level that they can be implemented.

As a general approach, it is recommended to define an object hierarchy to delineate the top-level conceptual structure (or architecture) of your system and then refine this with class hierarchies that specify concrete C ++ classes for your clients to use, three topics in this domain:
* Architecture design
* Class design
* Function design

## Architecture design

Software architecture describes the coarse structure of an entire system: the collection of top-level objects in the API and their relationships to each other.

### Developing an architecture

To come up with a good architecture design, one needs to produce a well-thought-out purposeful design that develivers a framework to implement the sytem and resolves trade-offs between the various conflicting requirements and constrains.

At a high level, the process of creating an architecture for an API resolves to four basic steps:
1. Analyze the functional requirements that affect the architecture.
2. Identify and account for the constraints on the architecture.
3. Invent the primary objects in the system and their relationships.
4. Communicate and document the architecture.

Those steps will be iterated until arriving at a perfect design.











# Testing

## Reason to write test

* Increased confidence
* Ensuring backward compatibility
* Saving cost
* Codify uses cases
* Compliance assurance


## Type of API testing

1. White box test: test are developed with knowledge of the source code.
2. Black box test: test are developed based on product specifications and without any knowledge of the uderlying implementation.
3. Gray box testing: A combination of white box and black box testing.

APIs are not end-user applications: they can only be used by writing code that calls functions defined within a library file. As a result, several types of traditional software testing techniques are not applicable to API testing.

> API testing should involve a combination of unit and integration testing. Non-functional techniques can also be applied as appropriate, such as performance, concurrency, and security testing.

### Unit testing

