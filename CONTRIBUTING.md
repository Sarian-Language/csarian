# Contributing to Sarian

Before contributing, please read these guidelines carefully.

---

## 1. Rules

1. **AI / Automated Code Usage**

   * Contributions generated with AI or automated tools **are not allowed**.
   * You may use AI for assistance, but **you must not use AI-generated code**.

2. **Commit Messages**

   * Each commit must include a **title and a detailed list of changes**. Example format:

     ```
     Add parser for function declarations
     - Fix memory leak in lexer
     - Update language documentation
     ```
     
   * **Use the imperative mood** in the subject line.  
     - Example: `Fix (x)`
     - Avoid: `Fixed (x)`
   
   * Avoid vague commit messages like “fix stuff” or “update code”.

3. **Testing Before Submission**

   * Verify that all changes **compile and run correctly**.
   * Include brief tests demonstrating that your code works as intended.

4. **Developer Certificate of Origin (DCO)**

   * All commits must be signed off to certify compliance with the [DCO](DCO.txt).

   * This is done by adding a `Signed-off-by: Name <your@email.x>` line at the end of each commit message.

   * Use the following command to sign your commits automatically:

     ```
     git commit -s
     ```

---

## 2. Code Style

* Follow the language’s **style guide** [CODE_STYLE.md](CODE_STYLE.md)
* Before committing, make sure to run the command **“make format”** as explained in the style guide.
* Ensure **readability and maintainability**, future contributors should be able to understand your code easily.

---

## 3. Reporting Issues

* Open issues for **bugs, feature requests, or improvements**.
* Check for existing issues before creating a new one to avoid duplicates.

---
