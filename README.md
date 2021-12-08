# Relatório do laboratório 3


Processador: Amd Ryzen 5 5600
Ram:16gb 3200mhz

# n = 10^5:

1 thread: aceleração = 116/345 = 0.33623188405  O alto custo(overhead) das threads com 'n' pequeno justifica a baixa aceleração, aprox 33% da velocidade sequencial na versão concorrente

<img src="https://cdn.discordapp.com/attachments/312753203668320256/917967299460358205/unknown.png"/>

2 threads: aceleração = 103/296 = 0.34797297297  similiar aos testes de 1 thread


4 threads: aceleração = 509/312 = 1.63141025641  Com 4 threads, vários casos demonstravam uma aceleração acima de 1, mas alguns ainda tinham aceleração abaixo de 1, com 'n' pequeno, o uso de threads pode não ser tão efetivo para ganho de desempenho.


<img src="https://cdn.discordapp.com/attachments/312753203668320256/917968533785301112/unknown.png"/>

# n = 10^7:

1 thread: aceleração = 12894/13203 = 0.97659622813 um 'n' maior acaba diminuindo a diferença entre o custo das threads e o processamento da tarefa em si, nesse caso, utilizar mais threads irá melhorar o desempenho em processadores multicores


2 threads: aceleração = 11733/6705 = 1.74988814318     um ganho de desempenho efetivo.

4 threads: aceleração = 11840/3982 = 2.97338021095    ganho maior ainda 

<img src="https://cdn.discordapp.com/attachments/312753203668320256/917970960479551518/unknown.png"/>


# n = 10^8:

1 thread: aceleração = 85809/93370 = 0.91902109885 

2 threads: aceleração = 117426/71985 = 1.63125651177

4 threads: aceleração = 2.5470984747 = 2.5470984747

No geral, resultados parecidos com os testes de n^7, estando a maior diferença nos tempos, e não nas acelerações


<img src="https://cdn.discordapp.com/attachments/312753203668320256/917973367724838962/unknown.png"/>
