#ifndef PyHP_CORE_H
# define PyHP_CORE_H

# define PyHP_LOW_API
# define PyHP_HIGH_API

# define PyHP_MAJOR_VERSION (0)
# define PyHP_MINOR_VERSION (0)
# define PyHP_MICRO_VERSION (0)
# define PyHP_RELEASE_LEVEL  PY_RELEASE_LEVEL_BETA
# define PyHP_RELEASE_SERIAL (0)

#define PyHP_VERSION_HEX ((PyHP_MAJOR_VERSION << 24) | \
            (PyHP_MINOR_VERSION << 16) | \
            (PyHP_MICRO_VERSION <<  8) | \
            (PyHP_RELEASE_LEVEL <<  4) | \
            (PyHP_RELEASE_SERIAL << 0))

#define PyHP_VERSION "0.0.0b0"


#endif /* PyHP_CORE_H */
