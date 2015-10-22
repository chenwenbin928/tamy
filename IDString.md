# IDString #

Sometimes you simply can't afford all the performance overhead tied to a regular string usage, yet you need to use strings.

IDString is a solution to that.

Under the hood, it pools all strings that were used to create IDString instances, assigning them simple integer ids, which from now on will be used instead, under the nice hood of IDString API.