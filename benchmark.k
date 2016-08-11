N: 100000000
M: 0$$_ 10^(0.25*1+!28)
numtrials: 10
i: 0
do[#M
  tm: 0
  do[#numtrials
    a: N _draw M[i]
    T0: _T
    b:<a
    tm+: 86400*(_T-T0)
  ]
  `0: ($M[i]),",",($tm%numtrials),"\n"
  i+:1
]
