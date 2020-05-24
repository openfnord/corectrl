# Contributing
CoreCtrl welcomes your contributions.

## Build instructions
You can find the build instructions on the [wiki](https://gitlab.com/corectrl/corectrl/wikis/Installation).

## Issues writting guidelines
Please, follow these guidelines to write your issues. Reported issues not following this guidelines may be closed as invalid.

* One issue per report.
* Be precise and clear.
* When available, use issues templates.

Please, as first step, read the [FAQ](https://gitlab.com/corectrl/corectrl/wikis/FAQ) and [Known issues](https://gitlab.com/corectrl/corectrl/wikis/Known-issues). If your issue doesn't appear on them, search for it on the issue tracker. If there is already an opened issue, add your input to it if you can aport new usefull info. If the issue is closed, open a new one and include a reference to the old one (for example, add 'Related to #<closed_issue_number>').

While writting your issue, use an issue template (if available) to populate the 'Description' field with important info. Adapt it to your needs.

Give to your issue a good descriptive title.

## Merge request
Before starting to work on a merge request, please follow these instructions:

1. Open an issue explaining the reason for the change, state that you want to work on it and wait for the developers response. Then, create a merge request from the issue and start working on the change.
2. Follow the [coding style](#coding-style) rules and make sure that your code integrates well into the code architecture.
3. Make sure that all unit tests pass when you are working with controls. If you are working on a new control, write a test for it.
4. During the merge request, be concise on your comments and make sure that you fully understand what you are stating on them.

## Coding style
* Format your code with clang-format. You can do it with your editor or running `./format.sh` on the root project directory, then pick the formated files that you are working on.
* Use [east const](https://mariusbancila.ro/blog/2018/11/23/join-the-east-const-revolution/).
* Use plain C++17 as much as possible. Don't use Qt functionality unless you have to.
