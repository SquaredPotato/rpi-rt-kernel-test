library(readr)
library(plotly)
times_rt <- read_csv("times-rt.csv")

times_rt$ACT_DUR = times_rt$ACT_LOW - times_rt$ACT_HIGH
times$ACT_DUR = times$ACT_LOW - times$ACT_HIGH

times_rt$PLS_SND = times_rt$PLS_SND / 1000000
times$PLS_SND = times$PLS_SND / 1000000

# Percentage of recorded pulses (real time patch version)
times_rt$REC_SND_DIFF = times_rt$PLS_REC / times_rt$PLS_SND * 100

# Plot of RT vs Non-RT pulse registering
plot_ly(times_rt, x=times_rt$ITER_CNT, name='Percentage of registered pulses') %>%
add_trace(y=times_rt$REC_SND_DIFF, mode='lines', type='scatter', name='RT') %>%
add_trace(y=times$REC_SND_DIFF, mode='lines', type='scatter', name='Non-RT') %>%
layout(xaxis = x, yaxis = y)

plot_ly(times_rt, x=times_rt$ITER_CNT, name='Percentage of registered pulses') %>%
  add_trace(y=times_rt$REC_SND_DIFF, mode='lines', type='scatter', name='RT') %>%
  add_trace(y=times$REC_SND_DIFF, mode='lines', type='scatter', name='Non-RT') %>%
  layout(xaxis = x, yaxis = y)