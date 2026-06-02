#include <iostream>
#include <string>
#include <limits>

using namespace std;

// ============================================================================
// CONFIGURACIONES Y CONSTANTES GLOBALES
// ============================================================================
const int MAX_PRODUCTOS = 50;

// ============================================================================
// ESTRUCTURAS DE DATOS (STRUCTS)
// ============================================================================
struct Producto {
    int codigo;
    string nombre;
    double precio;
    int stock;
};

struct NodoPedido {
    string nombreCliente;
    string detallePedido;
    double montoTotal;
    NodoPedido* siguiente;
};

struct NodoPago {
    int idTicket;
    double montoRecaudado;
    NodoPago* siguiente;
};

// ============================================================================
// PROTOCOLO DE VALIDACIÓN DE DATOS (ROBUSTEZ)
// ============================================================================
int leerEnteroPositivo(string mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "Ingreso invalido. Debe digitar un numero entero positivo.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double leerDecimalPositivo(string mensaje) {
    double valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= 0.0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "Ingreso invalido. Debe digitar un numero decimal positivo.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string leerCadena(string mensaje) {
    string cadena;
    while (true) {
        cout << mensaje;
        getline(cin, cadena);
        if (!cadena.empty() && cadena.find_first_not_of(" \t\n\v\f\r") != string::npos) {
            return cadena;
        }
        cout << "El campo no puede quedar vacio ni contener solo espacios.\n";
    }
}

// ============================================================================
// MÓDULO 1: GESTIÓN DE INVENTARIO (ARREGLO ESTÁTICO ORDENADO)
// ============================================================================
int busquedaBinaria(Producto inventario[], int cantidad, int codigoBuscado) {
    int izquierda = 0;
    int derecha = cantidad - 1;

    while (izquierda <= derecha) {
        int mitad = izquierda + (derecha - izquierda) / 2;

        if (inventario[mitad].codigo == codigoBuscado) {
            return mitad;
        }
        if (inventario[mitad].codigo < codigoBuscado) {
            izquierda = mitad + 1;
        } else {
            derecha = mitad - 1;
        }
    }
    return -(izquierda + 1);
}

void guardarOModificarProducto(Producto inventario[], int &cantidad) {
    cout << "\n--- REGISTRAR / MODIFICAR PRODUCTO ---\n";
    int codigo = leerEnteroPositivo("Ingrese el codigo del producto: ");
   
    int indice = busquedaBinaria(inventario, cantidad, codigo);

    if (indice >= 0) {
        cout << "Producto encontrado (" << inventario[indice].nombre << "). Actualizando datos.\n";
        inventario[indice].precio = leerDecimalPositivo("Nuevo Precio: S/. ");
        inventario[indice].stock = leerEnteroPositivo("Nuevo Stock disponible: ");
        cout << " Producto actualizado con exito.\n";
    } else {
        if (cantidad >= MAX_PRODUCTOS) {
            cout << " Capacidad limite del inventario alcanzada.\n";
            return;
        }
        int posInsercion = -indice - 1;
       
        for (int i = cantidad; i > posInsercion; i--) {
            inventario[i] = inventario[i - 1];
        }

        inventario[posInsercion].codigo = codigo;
        inventario[posInsercion].nombre = leerCadena("Nombre del producto: ");
        inventario[posInsercion].precio = leerDecimalPositivo("Precio: S/. ");
        inventario[posInsercion].stock = leerEnteroPositivo("Stock inicial: ");

        cantidad++;
        cout << "Nuevo producto registrado e insertado en orden.\n";
    }
}

void verCatalogoProductos(const Producto inventario[], int cantidad) {
    cout << "\n--- CATALOGO DE PRODUCTOS ---\n";
    if (cantidad == 0) {
        cout << "El inventario se encuentra vacio.\n";
        return;
    }
    cout << "ID/Cod.\t| Descripcion\t\t| Precio\t| Stock\n";
    cout << "-------------------------------------------------------------\n";
    for (int i = 0; i < cantidad; i++) {
        cout << inventario[i].codigo << "\t| "
             << inventario[i].nombre << "\t\t| S/. "
             << inventario[i].precio << "\t| "
             << inventario[i].stock << " und.\n";
    }
}

void eliminarProducto(Producto inventario[], int &cantidad) {
    cout << "\n--- ELIMINAR PRODUCTO ---\n";
    if (cantidad == 0) {
        cout << "[ERROR] No existen productos en el sistema.\n";
        return;
    }
    int codigo = leerEnteroPositivo("Ingrese el codigo del producto a eliminar: ");
    int indice = busquedaBinaria(inventario, cantidad, codigo);

    if (indice >= 0) {
        for (int i = indice; i < cantidad - 1; i++) {
            inventario[i] = inventario[i + 1];
        }
        cantidad--;
        cout << " Producto removido correctamente.\n";
    } else {
        cout << " El codigo ingresado no coincide con ningun producto.\n";
    }
}

// ============================================================================
// MÓDULO 2: ATENCIÓN DE PEDIDOS (COLA DINÁMICA FIFO)
// ============================================================================
void registrarPedido(NodoPedido* &frente, NodoPedido* &fin) {
    cout << "\n--- REGISTRAR NUEVO PEDIDO ---\n";
    NodoPedido* nuevoNodo = new NodoPedido();
   
    nuevoNodo->nombreCliente = leerCadena("Nombre del estudiante: ");
    nuevoNodo->detallePedido = leerCadena("Detalle del consumo: ");
    nuevoNodo->montoTotal = leerDecimalPositivo("Monto total a pagar: S/. ");
    nuevoNodo->siguiente = nullptr;

    if (frente == nullptr) {
        frente = nuevoNodo;
    } else {
        fin->siguiente = nuevoNodo;
    }
    fin = nuevoNodo;
    cout << " Pedido agregado al final de la fila.\n";
}

void apilarPago(NodoPago* &tope, double monto) {

    NodoPago* nuevoPago = new NodoPago();
    nuevoPago->idTicket = rand() % 9000 + 1000;
    nuevoPago->montoRecaudado = monto;

    nuevoPago->siguiente = tope;
    tope = nuevoPago;
}

void atenderSiguientePedido(NodoPedido* &frente, NodoPedido* &fin, NodoPago* &topePila) {
    cout << "\n--- ATENDER SIGUIENTE PEDIDO ---\n";
    if (frente == nullptr) {
        cout << "No hay estudiantes en la cola.\n";
        return;
    }

    NodoPedido* nodoAux = frente;
    cout << "Despachando a : " << nodoAux->nombreCliente << "\n";
    cout << "Pedido de     : " << nodoAux->detallePedido << "\n";
    cout << "Monto Cobrado : S/. " << nodoAux->montoTotal << "\n";

    apilarPago(topePila, nodoAux->montoTotal);

    frente = frente->siguiente;
    if (frente == nullptr) {
        fin = nullptr;
    }

    delete nodoAux;
    cout << " Pedido procesado y enviado a la caja.\n";
}

void verClientesEspera(NodoPedido* frente) {
    cout << "\n--- LISTA DE ESPERA ---\n";
    if (frente == nullptr) {
        cout << "No hay clientes en espera.\n";
        return;
    }
    NodoPedido* actual = frente;
    int posicion = 1;
    while (actual != nullptr) {
        cout << " Fila [" << posicion << "] -> " << actual->nombreCliente
             << " | Consumo: " << actual->detallePedido
             << " | Subtotal: S/. " << actual->montoTotal << "\n";
        actual = actual->siguiente;
        posicion++;
    }
}

// ============================================================================
// MÓDULO 3: HISTORIAL DE CAJA (PILA DINÁMICA LIFO)
// ============================================================================
void mostrarHistorialCaja(NodoPago* tope) {
    cout << "\n--- HISTORIAL DE CAJA ---\n";
    if (tope == nullptr) {
        cout << "Caja sin transacciones registradas.\n";
        return;
    }
    NodoPago* actual = tope;
    double totalAcumulado = 0.0;
    while (actual != nullptr) {
        cout << "  [TIQUE] #" << actual->idTicket << " | Ingreso: S/. " << actual->montoRecaudado << "\n";
        totalAcumulado += actual->montoRecaudado;
        actual = actual->siguiente;
    }
    cout << "-------------------------------------------------------------\n";
    cout << " SALDO NETO TOTAL EN CAJA: S/. " << totalAcumulado << "\n";
}

void cancelarUltimoPago(NodoPago* &tope) {
    cout << "\n--- ANULAR ULTIMA TRANSACCION ---\n";
    if (tope == nullptr) {
        cout << "El historial financiero se encuentra vacio.\n";
        return;
    }
    NodoPago* nodoAux = tope;
    cout << " Revirtiendo Tique #" << nodoAux->idTicket << " por: S/. " << nodoAux->montoRecaudado << "\n";
   
    tope = tope->siguiente;
    delete nodoAux;        
    cout << " Transaccion anulada exitosamente.\n";
}

void liberarMemoriaEstructuras(NodoPedido* &frente, NodoPago* &tope) {
    while (frente != nullptr) {
        NodoPedido* aux = frente;
        frente = frente->siguiente;
        delete aux;
    }
    while (tope != nullptr) {
        NodoPago* aux = tope;
        tope = tope->siguiente;
        delete aux;
    }
}

// ============================================================================
// INTERFAZ DE CONTROLADOR GENERAL (MENÚ EXPLICITO DE 4 OPCIONES)
// ============================================================================
int main() {
    Producto inventario[MAX_PRODUCTOS];
    int cantidadProductos = 0;
    NodoPedido* frenteCola = nullptr;
    NodoPedido* finCola = nullptr;
    NodoPago* topePila = nullptr;

    int opcionPrincipal = 0;

    do {
        cout << "\n============================================================\n";
        cout << "            SISTEMA INTEGRADO - CAFETIN DIGITAL             \n";
        cout << "============================================================\n";
        cout << " [1] Modulo: Gestion de Inventario\n";
        cout << " [2] Modulo: Ventas y Fila de Espera\n";
        cout << " [3] Modulo: Historial Financiero de Caja\n";
        cout << " [4] Cerrar Sistema y Salir\n";
        cout << "============================================================\n";
       
        opcionPrincipal = leerEnteroPositivo("Seleccione un modulo (1-4): ");

        switch (opcionPrincipal) {
            case 1: {
                int opcionSub = 0;
                do {
                    cout << "\n----------------------------------------------------\n";
                    cout << "          SUBMENU: GESTION DE INVENTARIO\n";
                    cout << "----------------------------------------------------\n";
                    cout << " [1] Registrar / Modificar Producto\n";
                    cout << " [2] Visualizar Catalogo Completo\n";
                    cout << " [3] Eliminar Producto\n";
                    cout << " [4] Volver al Menu Principal\n";
                    cout << "----------------------------------------------------\n";
                    opcionSub = leerEnteroPositivo("Seleccione una operacion (1-4): ");
                   
                    if (opcionSub == 1) guardarOModificarProducto(inventario, cantidadProductos);
                    else if (opcionSub == 2) verCatalogoProductos(inventario, cantidadProductos);
                    else if (opcionSub == 3) eliminarProducto(inventario, cantidadProductos);
                    else if (opcionSub != 4) cout << "Opcion invalida.\n";
                } while (opcionSub != 4);
                break;
            }
            case 2: {
                int opcionSub = 0;
                do {
                    cout << "\n----------------------------------------------------\n";
                    cout << "          SUBMENU: VENTAS Y ATENCION\n";
                    cout << "----------------------------------------------------\n";
                    cout << " [1] Registrar Pedido de Estudiante (Encolar)\n";
                    cout << " [2] Atender Siguiente Pedido (Desencolar)\n";
                    cout << " [3] Ver Clientes en Fila de Espera\n";
                    cout << " [4] Volver al Menu Principal\n";
                    cout << "----------------------------------------------------\n";
                    opcionSub = leerEnteroPositivo("Seleccione una operacion (1-4): ");
                   
                    if (opcionSub == 1) registrarPedido(frenteCola, finCola);
                    else if (opcionSub == 2) atenderSiguientePedido(frenteCola, finCola, topePila);
                    else if (opcionSub == 3) verClientesEspera(frenteCola);
                    else if (opcionSub != 4) cout << " Opcion invalida.\n";
                } while (opcionSub != 4);
                break;
            }
            case 3: {
                int opcionSub = 0;
                do {
                    cout << "\n----------------------------------------------------\n";
                    cout << "          SUBMENU: CONTROL DE CAJA\n";
                    cout << "----------------------------------------------------\n";
                    cout << " [1] Mostrar Transacciones y Balance Neto\n";
                    cout << " [2] Cancelar Ultima Operación (Desapilar)\n";
                    cout << " [3] Volver al Menu Principal\n";
                    cout << "----------------------------------------------------\n";
                    opcionSub = leerEnteroPositivo("Seleccione una operacion (1-3): ");
                   
                    if (opcionSub == 1) mostrarHistorialCaja(topePila);
                    else if (opcionSub == 2) cancelarUltimoPago(topePila);
                    else if (opcionSub != 3) cout << "Opcion invalida.\n";
                } while (opcionSub != 3);
                break;
            }
            case 4:
                liberarMemoriaEstructuras(frenteCola, topePila);
                cout << "\n Memoria dinámica liberada correctamente.\n";
                cout << "Cierre de sesión finalizado con exito.\n";
                break;
            default:
                cout << "Opción no valida. Use valores en el rango de 1 a 4.\n";
        }
    } while (opcionPrincipal != 4);

    return 0;
}