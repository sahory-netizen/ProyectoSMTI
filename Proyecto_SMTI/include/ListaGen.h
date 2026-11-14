#ifndef LISTAGEN_H
#define LISTAGEN_H

#include <iostream>

template <typename T>
struct NodoGen {
    T dato;
    NodoGen<T>* siguiente;
    NodoGen(T _d) : dato(_d), siguiente(nullptr) {}
};

// Lista enlazada generica. Se usa con punteros (ej: ListaGen<Sensor*>)
// y es la duena de los objetos: los borra en el destructor.
template <typename T>
class ListaGen {
private:
    NodoGen<T>* head;
    int tam;

public:
    ListaGen() : head(nullptr), tam(0) {}

    // Si se copiara la lista, dos listas borrarian los mismos objetos
    // (double free), por eso se prohibe la copia
    ListaGen(const ListaGen&) = delete;
    ListaGen& operator=(const ListaGen&) = delete;

    ~ListaGen() {
        NodoGen<T>* actual = head;
        while (actual != nullptr) {
            NodoGen<T>* aux = actual->siguiente;
            delete actual->dato;   // libera el objeto (llama al destructor virtual)
            delete actual;         // libera el nodo
            actual = aux;
        }
        head = nullptr;
    }

    // inserta al inicio
    void insertar(T nuevoDato) {
        NodoGen<T>* nuevo = new NodoGen<T>(nuevoDato);
        nuevo->siguiente = head;
        head = nuevo;
        tam++;
    }

    // devuelve el sensor con ese id, o nullptr si no existe
    T buscarPorId(int id) const {
        NodoGen<T>* actual = head;
        while (actual != nullptr) {
            if (actual->dato->getId() == id) {
                return actual->dato;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // recorre la lista y llama procesarLectura() de cada sensor
    void procesarTodo() {
        NodoGen<T>* actual = head;
        while (actual != nullptr) {
            actual->dato->procesarLectura();   // enlace dinamico
            actual = actual->siguiente;
        }
    }

    int getTam() const {
        return tam;
    }
};

#endif
