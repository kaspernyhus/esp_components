# A collection of esp-idf compatible components
![Static Badge](https://img.shields.io/badge/esp--idf->=v5.0-blue)


## How to include in an esp-idf project
To include a software component automatically in a esp-idf project, you have to define the dependencies in a yaml file called 'idf_component.yml'

To include s component add the following to `main/idf_component.yml`:

```
## IDF Component Manager Manifest File
dependencies:

# For components in git repository:
  component:
    path: "./component"
    version: "*"
    git: "git@github.com:<URL>"
```

when building the project this repo will automatically be downloaded and put in a `managed_components` directory inside the project.
