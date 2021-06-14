#ifndef PY3HP_CORE_H
# define PY3HP_CORE_H

# define PY3HP_LOW_API
# define PY3HP_HIGH_API

# define Py3hp_MAJOR_VERSION (0)
# define Py3hp_MINOR_VERSION (0)
# define Py3hp_MICRO_VERSION (0)
# define Py3hp_RELEASE_LEVEL  PY_RELEASE_LEVEL_BETA
# define Py3hp_RELEASE_SERIAL (0)

#define Py3hp_VERSION_HEX ((Py3hp_MAJOR_VERSION << 24) | \
            (Py3hp_MINOR_VERSION << 16) | \
            (Py3hp_MICRO_VERSION <<  8) | \
            (Py3hp_RELEASE_LEVEL <<  4) | \
            (Py3hp_RELEASE_SERIAL << 0))

#define Py3hp_VERSION "0.0.0b0"


#endif /* PY3HP_CORE_H */
