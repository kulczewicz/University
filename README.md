# SO-pracownia3

Implementacja części biblioteki pthreads do obsługi wątków z semaforami.

W swojej implementacji będę korzystał z koncepcji context switch, czyli będę zapisywać stan każdego procesu poprzez zapisanie process stack, program counter oraz signal mask, dlatego skorzystam ze struktury ucontext_t. Zaimplementuję algorytm round robin, żeby równomiernie przydzielić czas procesora do procesów. Żeby skutecznie przełączać się między procesami muszę przełączać się pomiędzy kontekstami i dlatego zaimplementuję funkcję, która zapisuje kontekst procesu przed przełączeniem się na inny proces, oraz funkcję, która go przywraca.

Korzystając z tych narzędzi zaimplementuję następujące funkcje:
 - void thread_libinit() - do inicjalizacji biblioteki
 - void thread_create(void func(void *), void *arg) - do tworzenia nowego wątku, który będzie wykonywał funkcję, podaną w pierwszym argumencie i przekaże do niej argumenty, które będą podane dalej
 - int thread_mutex_init(unsigned int lock) - funkcja, która tworzy mutex
 - int thread_mutex_lock(unsigned int lock) - funkcja, która ustawia mutex na aktualny proces.
 - void thread_mutex_unlock(unsigned int lock) - funkcja, która zdejmuje mutex z aktualnego procesu.
