import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog
from PIL import Image

# Function to hide a message within an image with password protection
def hide_message():
    image_path = image_entry.get()
    message = message_entry.get("1.0", tk.END).strip()
    password = simpledialog.askstring("Password", "Enter encryption password:")

    if not image_path:
        messagebox.showerror("Error", "Please select an image.")
        return
    if not message:
        messagebox.showerror("Error", "Please enter a message.")
        return
    if not password:
        messagebox.showerror("Error", "Please enter a password.")
        return

    try:
        img = Image.open(image_path)
        output_path = filedialog.asksaveasfilename(defaultextension=".png", filetypes=[("PNG files", "*.png")])
        if not output_path:
            return

        # Hide message in the image
        hide_message_in_image(img, message, password, output_path)
        messagebox.showinfo("Success", "Message hidden successfully!")
    except Exception as e:
        messagebox.showerror("Error", str(e))

# Function to hide a message within an image
def hide_message_in_image(img, message, password, output_path):
    width, height = img.size

    # Convert message to binary
    binary_message = ''.join(format(ord(char), '08b') for char in message)
    message_length = len(binary_message)

    # Encrypt message with password
    encrypted_message = encrypt_message(binary_message, password)

    # Check if message can fit in the image
    if len(encrypted_message) > width * height * 3:
        raise ValueError("Message is too long to be hidden in the image")

    index = 0
    for y in range(height):
        for x in range(width):
            pixel = list(img.getpixel((x, y)))
            for i in range(3):  # For each RGB component
                if index < len(encrypted_message):
                    # Modify the least significant bit
                    pixel[i] = pixel[i] & ~1 | int(encrypted_message[index])
                    index += 1
            img.putpixel((x, y), tuple(pixel))

    # Save the modified image
    img.save(output_path)

# Function to extract a hidden message from an image with password protection
def extract_message():
    extraction_window = tk.Toplevel(root)
    extraction_window.title("Extract Message")

    image_label = tk.Label(extraction_window, text="Image Path:")
    image_label.grid(row=0, column=0, padx=5, pady=5)
    image_entry_ex = tk.Entry(extraction_window, width=50)
    image_entry_ex.grid(row=0, column=1, padx=5, pady=5)
    image_browse_button = tk.Button(extraction_window, text="Browse", command=lambda: image_entry_ex.insert(tk.END, filedialog.askopenfilename()))
    image_browse_button.grid(row=0, column=2, padx=5, pady=5)

    password_label = tk.Label(extraction_window, text="Password:")
    password_label.grid(row=1, column=0, padx=5, pady=5)
    password_entry = tk.Entry(extraction_window, show="*", width=50)
    password_entry.grid(row=1, column=1, padx=5, pady=5)

    extract_button = tk.Button(extraction_window, text="Extract Message", command=lambda: do_extraction(image_entry_ex.get(), password_entry.get()))
    extract_button.grid(row=2, column=0, columnspan=3, padx=5, pady=5)

    # Text area to display the extracted message
    result_label = tk.Label(extraction_window, text="Extracted Message:")
    result_label.grid(row=3, column=0, padx=5, pady=5, sticky="w")
    result_text = tk.Text(extraction_window, width=50, height=5)
    result_text.grid(row=3, column=1, padx=5, pady=5, columnspan=2)

def do_extraction(image_path, password):
    if not image_path:
        messagebox.showerror("Error", "Please select an image.")
        return
    if not password:
        messagebox.showerror("Error", "Please enter a password.")
        return

    try:
        img = Image.open(image_path)
        message = extract_message_from_image(img, password)
        result_text.delete("1.0", tk.END)  # Clear previous content
        result_text.insert(tk.END, message)
    except Exception as e:
        messagebox.showerror("Error", str(e))

# Function to extract a hidden message from an image
def extract_message_from_image(img, password):
    width, height = img.size

    binary_message = ""
    for y in range(height):
        for x in range(width):
            pixel = img.getpixel((x, y))
            for i in range(3):
                # Extract the least significant bit
                binary_message += str(pixel[i] & 1)

    # Decrypt message with password
    decrypted_message = decrypt_message(binary_message, password)

    # Convert binary message to ASCII
    message = ""
    for i in range(0, len(decrypted_message), 8):
        byte = decrypted_message[i:i+8]
        message += chr(int(byte, 2))

    return message

# Function to encrypt message with password
def encrypt_message(message, password):
    encrypted_message = ""
    for i, char in enumerate(message):
        encrypted_message += str(ord(char) ^ ord(password[i % len(password)]))
    return encrypted_message

# Function to decrypt message with password
def decrypt_message(message, password):
    decrypted_message = ""
    for i, char in enumerate(message):
        decrypted_message += chr(int(char) ^ ord(password[i % len(password)]))
    return decrypted_message

# Create the main window
root = tk.Tk()
root.title("Steganography")

# Create widgets
image_label = tk.Label(root, text="Image Path:")
image_label.grid(row=0, column=0, padx=5, pady=5)
image_entry = tk.Entry(root, width=50)
image_entry.grid(row=0, column=1, padx=5, pady=5)
image_browse_button = tk.Button(root, text="Browse", command=lambda: image_entry.insert(tk.END, filedialog.askopenfilename()))
image_browse_button.grid(row=0, column=2, padx=5, pady=5)

message_label = tk.Label(root, text="Message:")
message_label.grid(row=1, column=0, padx=5, pady=5)
message_entry = tk.Text(root, width=50, height=5)
message_entry.grid(row=1, column=1, padx=5, pady=5)

encryption_password_label = tk.Label(root, text="Encryption Password:")
encryption_password_label.grid(row=2, column=0, padx=5, pady=5)
encryption_password_entry = tk.Entry(root, show="*", width=50)
encryption_password_entry.grid(row=2, column=1, padx=5, pady=5)

hide_button = tk.Button(root, text="Hide Message", command=hide_message)
hide_button.grid(row=3, column=0, columnspan=3, padx=5, pady=5)

extract_button = tk.Button(root, text="Extract Message", command=extract_message)
extract_button.grid(row=4, column=0, columnspan=3, padx=5, pady=5)

# Run the main event loop
root.mainloop()
