kssum: 2:["./libksimd.so";("kssum";1)]

eval:{[N;M]
  numtrials: 100

  kstime: 0
  bitime: 0
  do[numtrials
      a: ( N _draw 2*M )-M
      T0: _T
      ksresult:kssum[a]
      kstime+: 86400*(_T-T0)


      T0: _T
      bresult:+/a
      bitime+: 86400*(_T-T0)
  ]
  `0:($N),"\t",($M),"\t",($$(ksresult=bresult)),"\t",($bitime%numtrials),"\t",($kstime%numtrials),"\n"
}

N@:&9 < N: _ 10^0.25*1+!36
`0:"N\tM\tvalid\tBuiltin\tSIMD\n"
trash: eval[;1000]'N
_exit[0]
