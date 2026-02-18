import serial

# Configuration identique à Termite
ser = serial.Serial(
    port='COM7',  #port
    baudrate=1200,  # vitesse (Historique)
    bytesize=serial.SEVENBITS,  # 7 bits
    parity=serial.PARITY_EVEN,  # Parité Paire
    stopbits=serial.STOPBITS_ONE,
    timeout=1
)

print("Lecture du Linky en cours...")

while True:
    try:
        # Lire une ligne et décoder (ASCII 7 bits)
        line = ser.readline()
        # ignore les erreurs de décodage
        decoded_line = line.decode('ascii', errors='ignore').strip()

        # Si la ligne n'est pas vide
        if decoded_line:
            # On sépare l'étiquette de la valeur (ex: "PAPP 00150")
            parts = decoded_line.split()

            # Vérification basique qu'on a bien une étiquette et une valeur
            if len(parts) >= 2:
                label = parts[0]
                value = parts[1]

                # Filtrage des caractères de contrôle
                if label in ['PAPP', 'BASE', 'IINST', 'ISOUSC']:
                    print(f"Capteur: {label} | Valeur: {value}")

    except Exception as e:
        print(f"Erreur: {e}")