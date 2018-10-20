//Требуется написать программу, которая способна вычислять арифметические выражения.
//Выражения могут содержать:
//1) знаки операций '+', '-', '/', '*'
//2) Скобки '(', ')'
//3) Целые числа, в нотации '123', в том числе они достаточно большие (требуется реализовать длинную арифметику), все операции должны быть целочисленные
//4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего не значат
//5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "[error]" (без кавычек) 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ERROR_MESSAGE "[error]"
#define ERROR_SIGNAL -1
#define CLONG_SIZE 30
#define BASE 10

int priority(char sign) {
  switch(sign) {
    case '+':
      return 0;
    case '-':
      return 0;
    case '*':
      return 1;
    case '/':
      return 1;
    case '(':
      return -1;
    case '~':
      return 2;
    default:
      return -9;
  }
}

int issign(char sign) {
  switch(sign) {
    case '+':
      return 1;
    case '*':
      return 1;
    case '/':
      return 1;
    case '-':
      return 1;
    case '~':
      return 1;
    default:
      return 0;
  }
}

int genMemoryError(char * pointer) {
  if(pointer)
    free(pointer);
  return ERROR_SIGNAL;
}

int errorf(char * pointer) {
  printf(ERROR_MESSAGE);
  return genMemoryError(pointer);
}

// CLong - структура данных и функции для работы с длинной арифметикой
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CLong {
  size_t size;
  short buffer[CLONG_SIZE];
};

void initCLong(struct CLong * object, size_t size) {
  memset(object->buffer, 0, sizeof(object->buffer));
  object->size = size;
}

void copyFrom(struct CLong * A, struct CLong * const B) {
  for(size_t i = 0; i < B->size; i++) {
    A->buffer[i] = B->buffer[i];
  }
  A->size = B->size;
}

// Функция для перевода числа в строковом представлении в CLong
struct CLong toCLong(char * const number, size_t size) {
  struct CLong result;
  initCLong(&result, size);
  for(int i = size - 1; i >= 0; i--) {
    (result.buffer)[size - 1 - i] = (short)(number[i] - '0');
  }
  return result;
}

// Меняет знак числа
void unaryMinus(struct CLong * A) {
  for(size_t i = 0; i < A->size; i++) {
    A->buffer[i] = -(A->buffer[i]);
  }
}

// Возвращает отрицательную копию числа
struct CLong unaryMinusCopy(struct CLong * const A) {
  struct CLong result;
  initCLong(&result, A->size);
  for(size_t i = 0; i < A->size; i++) {
    result.buffer[i] = -(A->buffer[i]);
  }
  return result;
}

// Проверка на знак
int isNegative(struct CLong * const A) {
  for(size_t i = 0; i < A->size; i++) {
    if(A->buffer[i] < 0)
      return 1;
  }
  return 0;
}

// Возвращает модуль значения
struct CLong  moduleCopy(struct CLong * const A) {
  struct CLong result;
  initCLong(&result, A->size);
  for(size_t i = 0; i < A->size; i++) {
    result.buffer[i] = abs(A->buffer[i]);
  }
  return result;
}

// Функция сравнения по модулю |A| > |B|
int compare(struct CLong * const A, struct CLong * const B) {
  if(A->size > B->size) {
    return 1;
  }
  else if(A->size < B->size) {
    return 0;
  }
  else {
    int sum1 = 0;
    int sum2 = 0;
    for(size_t i = 0; i < A->size; i++) {
      sum1 += A->buffer[i];
    }
    if(sum1 < 0) {
      sum1 *= -1;
    }
    for(size_t i = 0; i < B->size; i++) {
      sum2 += B->buffer[i];
    }
    if(sum2 < 0) {
      sum2 *= -1;
    }
    if(sum1 > sum2) {
      return 1;
    }
    else {
      return 0;
    }
  }
}

// A-B
struct CLong SUB(struct CLong * const A, struct CLong * const B);
// A+B
struct CLong SUM(struct CLong * const A, struct CLong * const B) {
  unsigned long i = 0l;
  long temp = 0l;
  short carry = 0;
  struct CLong result;
  initCLong(&result,0);
  if(isNegative(A) && isNegative(B)) {
    struct CLong Bcpy = moduleCopy(B);
    struct CLong Acpy = moduleCopy(A);
    result = SUM(&Acpy, &Bcpy);
    unaryMinus(&result);
    return result;
  }
  if((isNegative(A) + isNegative(B))%2) {
    struct CLong Bcpy = moduleCopy(B);
    struct CLong Acpy = moduleCopy(A);
    if(isNegative(A)) {
      if(compare(A, B)) { 
        result =  SUB(&Acpy,&Bcpy);
        unaryMinus(&result);
        return result;
      }
      else {
        return SUB(&Bcpy,&Acpy);
      } 
    } 
    else {
      if(compare(A,B)) { 
        return  SUB(&Acpy,&Bcpy);
      }
      else {
        result = SUB(&Bcpy,&Acpy);
        unaryMinus(&result);
        return result;
      } 
    }
  }
  if(A->size < B->size) {
    return SUM(B,A);
  }
  for(; i < B->size; i++) {
    temp = A->buffer[i] + B->buffer[i] + carry;
    if(temp >= BASE) {
      result.buffer[i] = temp - BASE;
      carry = 1;
    }
    else {
      result.buffer[i] = temp;
      carry = 0;
    }
  }
  for(; i < A->size; i++) {
    temp = A->buffer[i] + carry;
    if(temp >= BASE) {
      result.buffer[i] = temp - BASE;
      carry = 1;
    }
    else {
      result.buffer[i] = temp;
      carry = 0;
    }
  }
  if(carry) {
    result.buffer[i] = carry;
    result.size = A->size + 1;
  }
  else result.size = A->size;
  return result;
}
// A - B
struct CLong SUB(struct CLong * const A, struct CLong * const B) {
  struct CLong result;
  initCLong(&result,0);
  if(compare(B, A)) { 
    result = SUB(B,A);
    unaryMinus(&result);
    return result;
  } 
  if(isNegative(A) && !isNegative(B)) {
    struct CLong Acpy = unaryMinusCopy(A);
    result = SUM(&Acpy,B);
    unaryMinus(&result);
    return result;
  }
  else if(isNegative(A) && isNegative(B)) {
    struct CLong Acpy = unaryMinusCopy(A);
    struct CLong Bcpy = unaryMinusCopy(B);
    result = SUB(&Acpy, &Bcpy);
    unaryMinus(&result);
    return result;
  } else if (isNegative(B)) {
    struct CLong Bcpy = unaryMinusCopy(B);
    return SUM(A,&Bcpy);
  }
  unsigned long i = 0;
  long temp, carry = 0;
  for(; i < B->size; i++) {
    temp = A->buffer[i] - B->buffer[i] + carry;
    if(temp < 0) {
      result.buffer[i] = temp + BASE;
      carry = -1;
    }
    else {
      result.buffer[i] = temp;
      carry = 0;
    }
  }
  for(; i < A->size; i++) {
    temp = A->buffer[i] + carry;
    if(temp < 0) {
      result.buffer[i] = temp + BASE;
      carry = -1;
    }
    else {
      result.buffer[i] = temp;
      carry = 0;
    }
  }
  i = A->size - 1;
  while((i > 0) && (result.buffer[i] == 0)) i--;
  result.size = i + 1;
  return result;
}

// A*b  b - short
struct CLong SMUL(struct CLong * const A, short B) {
  unsigned long i = 0l;
  long temp = 0l;
  short carry = 0;
  struct CLong result;
  initCLong(&result,0);
  for(; i < A->size; i++) {
    temp = A->buffer[i] * B + carry;
    carry = temp / BASE;
    result.buffer[i] = temp - carry * BASE;
  }
  if(carry) {
    result.buffer[i] = carry;
    result.size = A->size + 1;
  }
  else result.size = A->size;
  return result;
}

// A * B
struct CLong MUL(struct CLong * const A, struct CLong * const B) {
  unsigned long i, j;
  long temp, carry;
  struct CLong result;
  initCLong(&result,0);
  for(i = 0; i < A->size; i++) {
    carry = 0;
    for(j = 0; j < B->size; j++) {
      temp = A->buffer[i] * B->buffer[j] + result.buffer[i + j] + carry;
      carry = temp / BASE;
      result.buffer[i + j] = temp - carry * BASE;
    }
    result.buffer[i + j] = carry;
  }

  i = A->size + B->size - 1;
  if(result.buffer[i] == 0) i--;
  result.size = i + 1;
  return result;
}

// A / b b - short
struct CLong SDIV(struct CLong * const A, short B) {	
  struct CLong result;
  initCLong(&result,0);
  short r = 0;
  long i;
  long temp = 0l;
  for(i = A->size - 1; i >= 0; i--) {
    temp = r * 1l * BASE + A->buffer[i];
    result.buffer[i] = temp / B;
    r = temp - result.buffer[i] * B;
  }
  i = A->size - 1;
  while((i > 0) && (result.buffer[i] == 0)) i--;
  result.size = i + 1;
  return result;
}

// A / B - как я запарился эту жесть писать 
struct CLong DIV(struct CLong * const A, struct CLong * B) {	
  struct CLong Q;
  initCLong(&Q, 1);
  struct CLong R;
  initCLong(&R, 0);
  if(B->size > A->size) {
    return Q;
  }
  if(B->size == 1) {
    return SDIV(A, B->buffer[0]);
  }
  struct CLong U;
  initCLong(&U, 0);
  copyFrom(&U, A);
  U.size = A->size + 1;
  U.buffer[A->size] = 0;
  long n = B->size, m = U.size - B->size;
  long uJ = 0, vJ = 0, i = 0;
  long temp1 = 0, temp2 = 0, temp = 0;
  short scale = 0;
  short qGuess = 0, r = 0;
  short borrow = 0, carry = 0;
  scale = BASE / (B->buffer[n-1] + 1);
  if(scale > 1) {
    U = SMUL(&U, scale);
    *B = SMUL(B, scale);
  }
  for(vJ = m, uJ = n + vJ; vJ >= 0; --vJ, --uJ) {
    qGuess = (U.buffer[uJ] * BASE + U.buffer[uJ - 1]) / B->buffer[n - 1];
    r = (U.buffer[uJ] * BASE + U.buffer[uJ - 1]) % B->buffer[n-1];
    while(r < BASE) {
      temp2 = B->buffer[n - 2] * qGuess;
      temp1 = r * BASE + U.buffer[uJ - 2];
      if((temp2 > temp1) || (qGuess == BASE)) {
        --qGuess;
        r += B->buffer[n-1];
      }
      else break;
    }
    carry = 0;
    borrow = 0;
    short * UShift = U.buffer + vJ;
    for(i = 0; i < n; i++) {
      temp1 = B->buffer[i] * qGuess + carry;
      carry = temp1 / BASE;
      temp1 -= carry * BASE;
      temp2 = UShift[i] - temp1 + borrow;
      if( temp2 < 0 ) {
        UShift[i] = temp2 + BASE;
        borrow = -1;
      }
      else {
        UShift[i] = temp2;
        borrow = 0;
      }
    }
    temp2 = UShift[i] - carry + borrow;
    if(temp2 < 0) {
      UShift[i] = temp2 + BASE;
      borrow = -1;
    }
    else {
      UShift[i] = temp2;
      borrow = 0;
    }
    if(borrow == 0) {
      Q.buffer[vJ] = qGuess;
    }
    else {
      Q.buffer[vJ] = qGuess - 1;
      carry = 0;
      for(i = 0; i < n; i++) {
        temp = UShift[i] + B->buffer[i] + carry;
        if(temp >= BASE) {
          UShift[i] = temp - BASE;
          carry = 1;
        }
        else {
          UShift[i] = temp;
          carry = 0;
        }
      }
      UShift[i] = UShift[i] + carry - BASE;
    }
    i = U.size - 1;
    while((i > 0) && (U.buffer[i] == 0)) i--;
    U.size = i + 1;
  }
  while ((m > 0) && (Q.buffer[m] == 0)) m--;
  Q.size = m + 1;
  if(scale > 1) {
    *B = SDIV(B, scale);
    R = SDIV(&U, scale);
  }
  else R = U;

  return Q;
}

// Вывод длинного чила
void printCLong(struct CLong object) {
  // Выводим одно знаковое значение старшего разряда
  // остальные в абсолютном значение
  printf("%i",object.buffer[object.size - 1]);
  for(int i = object.size - 2; i >= 0; i--) {
    printf("%i",abs(object.buffer[i]));
  }
  printf("\n");
}

// A op B = C
struct CLong workAtTheStack(struct CLong * const A, struct CLong * const B, char operator) {
  switch(operator) {
    case '+':
      return SUM(A, B);
    case '-':
      return SUB(A, B);
    case '*':
      return MUL(A, B);
    case '/': { 
      struct CLong Acpy = moduleCopy(A);
      struct CLong Bcpy = moduleCopy(B);
      if((isNegative(A) + isNegative(B))%2) {
        Acpy = DIV(&Acpy, &Bcpy);
        unaryMinus(&Acpy);
        return Acpy;
      }
      return DIV(&Acpy, &Bcpy);
    }
    default: {
      struct CLong result;
      initCLong(&result, 0);
      return result;
    }
  }
}

//	CStack - стек для хранения строк
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CStack {
  char * stack;
  size_t capacity;
  size_t size;
};

void freeStack(struct CStack object) {
  if(object.stack) {
    free(object.stack);
  }
}

// realloc с проверкой
int reallocation(char** pointer, size_t * oldCapacity) {
  size_t newCapacity = (*oldCapacity) + (*oldCapacity)/2;
  char * handler = (char*)realloc((*pointer), newCapacity);
  if(!handler) 
    return errorf(*pointer);
  (*oldCapacity) = newCapacity;
  (*pointer) = handler;
  return 0;
} 

// calloc c проверкой
int checkCalloc(char** pointer, size_t size) {
  (*pointer) = (char*)calloc(size, sizeof(char));
  if(!(*pointer)) 
    return errorf(NULL);
  return 0;				 
}

// инициализация стека
int initCStack(struct CStack * object, size_t capacity) {
  object->size = 0;
  object->capacity = capacity;
  if(checkCalloc(&(object->stack), capacity))
    return ERROR_SIGNAL;
  return 0;
}

int push(struct CStack * object, char symbol) {
  if(object->stack) {
    if(object->capacity > object->size) {
      object->stack[(object->size)++] = symbol;
      return 0;
    }
    else if(!reallocation(&(object->stack), &(object->capacity))) {
      object->stack[(object->size)++] = symbol;
      return 0;
    }
  }
  return ERROR_SIGNAL;
}

char pop(struct CStack * object) {
  if(object->stack) {
    if(object->size) {
      return object->stack[--(object->size)];
    }
  }
  return '!';
}

// возращаем значени последнего элемента в стеке
char back(struct CStack const object) {
  if(object.size != 0) {
    return object.stack[object.size - 1];
  }
  return ' ';
}

// копирование из одного стека в другой
int fromToAnother(struct CStack * first, struct CStack * second) {
  while(first->size) {
    if(push(second, pop(first)))
      return errorf(NULL);
  }
  return 0;
}

int isBrace(char symbol) {
  if(symbol == '(' || symbol == ')')
    return 1;
  return 0;
}

int createRPN(char * const buffer, struct CStack ** RPN, size_t * capacityRPN, size_t * sizeRPN) {	
  // worker - служебный стек
  // если бы читали сразу в RPN, то числа были бы перевернуты
  // поэтому решил для  CLong более привычный формат сделать
  struct CStack worker;
  if(initCStack(&worker, CLONG_SIZE))
    return ERROR_SIGNAL;
  // num стек для записи цифер числа
  // запишет число 123 как 321
  // потом оттуда с помощью воркера пишем в RPN как 123
  struct CStack num;
  if(initCStack(&num, CLONG_SIZE))
    return ERROR_SIGNAL;
  // стек для операторов
  struct CStack operators;
  if(initCStack(&operators, strlen(buffer)/2 + 1))
    return ERROR_SIGNAL;
  char const * iterator = NULL;
  for(iterator = buffer; *iterator != '\0'; iterator++) {	
    // пропускаем пробелы
    if(!isspace(*iterator)) {
      // если встречаем оператор или скобку, то пишем num -> worker -> RPN
      if(issign(*iterator) || isBrace(*iterator)) {	
        if(num.size) {	
          fromToAnother(&num, &worker);
          fromToAnother(&worker, &((*RPN)[*sizeRPN])); // except
          (*sizeRPN)++;
        }
      }
      // если число - пушим в num
      if(isdigit(*iterator)) {
        push(&num, *iterator); // except
      }
      else if(issign(*iterator)) {
        // если стек operators не пуст
        // и последний символ в стеке не '('
        // и приоритет последнего символа больше или равен *iterator
        // выбрасываем последний символ из стека операторов в RPN
        // и записываем в стек операторов *iterator
        // иначе просто записываем *iterator в стек операторов
        if(operators.size != 0 
            && back(operators) != '(' 
            && (priority(*iterator) <= priority(back(operators))))
        {	
          push(&((*RPN)[*sizeRPN]), pop(&operators)); //except
          (*sizeRPN)++;
          push(&operators, *iterator); //except
        }
        else {
          push(&operators, *iterator); //except
        }
      }	
      // если откр. скобка - кладем в оп. стек
      else if(*iterator == '(') {
        push(&operators, '('); // except
      }
      // если закрывающая скобка - выбрасываем все операторы
      // в стек RPN
      else if(*iterator == ')') {
        char temp = pop(&operators);
        while(temp != '(') {	
          push(&((*RPN)[*sizeRPN]), temp);  // exept
          (*sizeRPN)++;
          temp = pop(&operators);
          if(operators.size == 0 && temp != '(') {
            freeStack(operators);
            freeStack(num);
            freeStack(worker);
            return errorf(NULL);
          }
        }
      }
      // иначе это символ не из этой оперы
      // выбрасываем исключение
      else {
        freeStack(operators);
        freeStack(num);
        freeStack(worker);
        return errorf(NULL);
      }
    }
  }
  // если что-то в num осталось - кладем в RPN
  if(num.size) {
    fromToAnother(&num, &worker);
    fromToAnother(&worker, &((*RPN)[*sizeRPN]));
    (*sizeRPN)++;
  }
  // если что-то осталось в operators - кладем в RPN
  while(operators.size) {
    push(&((*RPN)[*sizeRPN]), pop(&operators)); //except
    (*sizeRPN)++;
  }
  freeStack(operators);
  freeStack(num);
  freeStack(worker);
  return 0;
}

// печать содержимого стека
void printCStack(struct CStack object) {
  if(object.stack) {
    for(size_t i = 0; i < object.size; i++)
      printf("%c", object.stack[i]);
    printf(" ");
  }
}

// чтение из стандартного ввода
int readFromStdInput(char ** pointer) {
  size_t BUFFER_SIZE = 1024;
  const size_t CHUNK_SIZE = 64;
  char chunk[CHUNK_SIZE];
  size_t i = 1;
  if(checkCalloc(pointer, BUFFER_SIZE)) {
    return ERROR_SIGNAL;
  }
  if(!fgets(chunk, CHUNK_SIZE, stdin)) {
    return errorf(*pointer);
  }
  for(; 1; i++) {
    if((BUFFER_SIZE - CHUNK_SIZE * i) != 1) {
      strcat((*pointer), chunk);
      chunk[0] = '\0';
    }
    else if(reallocation(pointer, &BUFFER_SIZE)) {
      return errorf(NULL);
    }
    else { 
      strcat((*pointer), chunk);
      chunk[0] = '\0';
    }
    if(feof(stdin) || fgets(chunk, CHUNK_SIZE, stdin)) break;
  }
  i *= CHUNK_SIZE;
  if(reallocation(pointer, &i)) 
    return errorf(NULL);
  return 0;
}

// преанализ строки
int preAnalysis(char ** buffer) {
  char * iterator = (*buffer);
  size_t capacityRPN = 0;
  for(; *iterator != '\0'; iterator++) {
    if(issign(*iterator) || isBrace(*iterator)) {
      capacityRPN++;
    }
    // если символ '-'
    // и (итератор стоит в начале строки
    // или предыдущий символ - оператор
    // или предыдущий символ - открывающаяся скобка)
    // меняем знак - на ~(унарный минус)
    if(*iterator == '-' && ( iterator == (*buffer) 
          || issign(*(iterator - 1)) 
          || *(iterator - 1) == '('))
    {
      *iterator = '~';
    }
  }
  // возвращаем возможный capacity из следующего предположения:
  // мы считаем количество операторов
  // если их n то операндов n + 1
  // и число m скобок
  return 2*capacityRPN + 4;
}

int calculate(struct CStack * const RPN, size_t sizeOfRPN) {
  struct CLong * numberStack = (struct CLong *)calloc(sizeOfRPN/2 + 2, sizeof(struct CLong));
  size_t sizeOfNumberStack = 0;
  if(!numberStack) {
    return errorf(NULL);
  }
  struct CLong result;
  initCLong(&result, 0);
  for(size_t i = 0; i < sizeOfRPN; i++) {
    if(isdigit(RPN[i].stack[0])) {
      numberStack[sizeOfNumberStack++] = toCLong(RPN[i].stack, RPN[i].size);
    } 
    else if(issign(RPN[i].stack[0])) {
      if(RPN[i].stack[0] != '~') {
        if(sizeOfNumberStack < 2) {
          free(numberStack);
          return errorf(NULL);
        }
        numberStack[sizeOfNumberStack - 2] =  workAtTheStack(&numberStack[sizeOfNumberStack - 2], &numberStack[sizeOfNumberStack - 1], RPN[i].stack[0]);
        sizeOfNumberStack--;
      }
      else {
        unaryMinus(&(numberStack[sizeOfNumberStack - 1]));
      }
    }
    else {
      free(numberStack);
      return errorf(NULL);		 
    }
  }
  if(sizeOfNumberStack > 1) {
    free(numberStack);
    return errorf(NULL);			
  }
  printCLong(numberStack[--sizeOfNumberStack]);
  free(numberStack);
  return 0;
}

int main() {
  char * buffer = NULL;
  if(readFromStdInput(&buffer))
    return 0; 
  size_t capacityRPN = preAnalysis(&buffer);
  size_t sizeRPN = 0;
  struct CStack * RPN = (struct CStack *)calloc(capacityRPN, sizeof(struct CStack));
  if(!RPN) {
    errorf(NULL);
    free(buffer);
    return 0;
  }
  for(size_t i = 0; i < capacityRPN; i++) {
    if(initCStack(RPN + i, 2)) {
      free(buffer);
      return 0;
    }
  }
  if(createRPN(buffer, &RPN, &capacityRPN, &sizeRPN)) {
    free(buffer);
    for(size_t i = 0; i < capacityRPN; i++) {
      freeStack(RPN[i]);
    }
    free(RPN);
    return 0;
  }
  calculate(RPN, sizeRPN);
  free(buffer);
  for(size_t i = 0; i < capacityRPN; i++) {
    freeStack(RPN[i]);
  }
  free(RPN);
  return 0;
}
