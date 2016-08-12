kb32sort: 2:["./libkb32sort.so";("kb32sort";1)]

eval:{[N;M]
  numtrials: 10

  kstime: 0
  bitime: 0
  do[numtrials
      a:  N _draw M 
      T0: _T
      ksresult:kb32sort[a]
      kstime+: 86400*(_T-T0)


      T0: _T
      bresult:<a
      bitime+: 86400*(_T-T0)
  ]
  bavg: bitime%numtrials
  avg:kstime%numtrials 
  imp: 100*((bavg-avg)%bavg)
  `0:($N),"\t",($M),"\t",($bavg),"\t",($avg),"\t",($imp),"\n"
}

N@:&9 < N: _ 10^0.5*1+!14
M: _ 10 ^ 1+!6
`0:"N\tM\tBuiltin\tb32sort\n"

i:0
j:0
do[#N
  j:0
  do[#M
    eval[N[i];M[j]]
    j+:1
  ]
  i+:1
]
_exit[0]
