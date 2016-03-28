Hansim
==============

A strategy simulator for the well-known cooperative card
game [Hanabi](https://boardgamegeek.com/boardgame/98778/hanabi).
This directory contains code for two strategies,
both using hat guessing to allow hints to be given
to each player simultaneously.  

The first strategy, contained in the files that begin 
with `rec_`, is an actionable clues based strategy.  The
second strategy, whose files are prefixed with `info_`,
comprise a more complex strategy that achieves a perfect
score 75% of the time.


Compilation
===============

With the GCC compiler, to compile the rec_ strategy, use
`g++ rec_main.cpp`
To compile the info_ strategy, use
`g++ info_main.cpp`

For other compilers, just build either rec_main.cpp or
info_main.cpp.


Credits
===============

This project began at the Rocky Mountain--Great Plains
Graduate Workshop in Combinatorics, 2014.  This code
accompanies a paper, which can be found:
* [on JSTOR](https://www.jstor.org/stable/10.4169/math.mag.88.5.323) in its completed form ($16)
* [on Google Groups](https://d0474d97-a-62cb3a1a-s-sites.googlegroups.com/site/rmgpgrwc/research-papers/Hanabi_final.pdf?attachauth=ANoY7cqtTi2kcdjhl_G05fBKiod69eLEN-4OF__V-UrXL4DJTRAUVcdNjfhduB5jqO4NvFgaNtrvZOOZMRe2eWvnYOBBi1re1zknJWstbHBihz0vlXQWINuqpOqdl2sMpoO5cWM1CdrOEV92KtnsAtJxDYXviGpjAqmlRC4T1N8peHfFs68TNO8Zgqluzx5SKrt8Qe7SNf0B1qxF3knrCLFnM8iA1rTdMxlKPB2zBeudv2W5cIbMd7A%3D&attredirects=0) in near-final draft form (free)

Strategy, code and paper were written by the GRWC Hanabi group:
Chris Cox, Jessica De Silva, Phil DeOrsey, Franklin Kenter, Troy Retter, Josh Tobin
