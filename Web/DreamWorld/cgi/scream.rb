#!/usr/bin/env ruby

# Définit le type de contenu de la réponse
puts "Content-type: text/html\n\n"

# Lire la valeur de la variable d'environnement 'toScream'
to_scream = ENV['toScream']

# Vérifier si 'toScream' a été fourni
if to_scream.nil? || to_scream.empty?
  screamed_data = "AUCUN TEXTE FOURNI!"
else
  # Convertir la valeur en majuscules
  screamed_data = to_scream.upcase
end

# Créer une réponse HTML simple avec les données criées
puts "<html>"
puts "<head>"
puts "<title>AAAAAAAAAAAAA</title>"
puts "</head>"
puts "<body>"
puts "<h1>AAAAAAAAAAAAAAA :</h1>"
puts "<p>#{screamed_data}</p>"
puts "<a href=\"/scream.html\">Retour au formulaire</a>"
puts "</body>"
puts "</html>"

