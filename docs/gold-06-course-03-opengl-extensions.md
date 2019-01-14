---
id: gold-06-course-03-opengl-extensions
title: Extensions OpenGL
---

Le méchanisme d'extensions OpenGL permet aux constructeurs de carte graphique d'ajouter à l'API des fonctionnalités avancées qui ne sont pas encore présentes dans le Core profile.

Il est assez standard d'utiliser des extensions OpenGL et d'adapter son moteur en fonction des extensions disponibles sur la carte graphique de l'utilisateur.

Le nom des extensions suit une nomenclature assez précises. Il y a 3 types d'extensions:

- Propriétaires: Spécifiques à un seul constructeur. Par exemple les extensions préfixées par GL_NV sont spécifiques aux GPU de NVidia.
- Génériques: Généralement implémentées par un grand nombre de constructeurs. Préfixées par GL_EXT
- ARB: Extensions génériques approuvées par l'[OpenGL ARB](https://www.opengl.org/archives/about/arb/), destinées à entrer dans le Core profile d'une future version d'OpenGL.

Les extensions sont détaillées dans un fichier texte dedié assez difficile à lire (exemple: https://www.opengl.org/registry/specs/ARB/vertex_array_object.txt) car il mentionne des additions, suppressions ou modifications dans la spécification générale d'OpenGL.

Plus d'infos sur le Wiki: https://www.khronos.org/opengl/wiki/OpenGL_Extension

Pour savoir si une extension OpenGL est proposée par votre driver, vous pouvez utiliser la commande *glxinfo sous Linux (*nvidia-config --glxinfo* sous certains système) et piper le résultat dans un grep du nom de l'extension recherchée. Sous windows vous pouvez passer par un utilitaire tel que [GLview](http://realtech-vr.com/admin/glview).

Quelques extensions très interessantes:

## Direct State Access

L'extension Direct State Access (DSA) ([GL_EXT_direct_state_access](https://www.opengl.org/registry/specs/EXT/direct_state_access.txt), puis [GL_ARB_direct_state_access](https://www.opengl.org/registry/specs/ARB/direct_state_access.txt), puis introduite en Core 4.5) propose une nouvelle manière de modifier/accéder aux objets OpenGL, sans avoir à les binder.

Cela permet d'éviter de se préoccuper de l'état global du contexte OpenGL lorsque l'on veut manipuler les objets OpenGL et ainsi d'éviter un grand nombre d'erreurs de programmation.

{{< notice "warning" >}}
Cette extension n'est pas disponible sur toutes les cartes graphiques. En particulier les machines de la FAC n'en sont pas equipées entièrement (il y a l'extension <strong>GL_EXT_direct_state_access</strong> qui n'est que partielle). Si vous n'y avez pas accès, ne l'utilisez pas: cette extension ne fournit que des facilités de programmation, pas de fonctionnalité en plus.
{{< /notice >}}

Cette extension est très pratique car elle permet d'éviter de binder les objets OpenGL pour les manipuler (on passe aux fonctions directement l'identifiant de l'objet), et donc d'éviter les erreurs liées au mécanisme de binding. Je vous conseille donc de l'utiliser autant que possible.

[Lien vers le document de référence](https://www.opengl.org/registry/specs/ARB/direct_state_access.txt).

Voici des exemples de code en version OpenGL 3 classique et en version Direct State Access (directement tiré du document):

## Example 1: Creating a buffer object without polluting the OpenGL states
```cpp
// Bind to Create
GLuint CreateBuffer()
{
  // Save the previous bound buffer
  GLuint restoreBuffer = 0;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &restoreBuffer);

  // Reserve the buffer name and create the buffer object
  uint buffer = 0;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  // Restaure the previous bound buffer to avoid polluting
  // the rendering states
  glBindBuffer(GL_ARRAY_BUFFER, restoreBuffer);

  return buffer;
}

// Direct State Access
GLuint CreateBuffer()
{
  GLuint buffer = 0;
  glCreateBuffer(1, &buffer);

  return buffer;
}
```

## Example 2: Creating a vertex array object without polluting the OpenGL states

```cpp
// OpenGL 3.0 Bind to Create for vertex array object
GLuint CreateVertexArray(GLuint BufferName[])
{
  // Save the previous bound vertex array and array buffer
  GLuint restoreVertexArray = 0;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
  GLuint restoreBuffer = 0;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &restoreBuffer);

  glGenVertexArrays(1, &VertexArrayName);
  glBindVertexArray(VertexArrayName);
    glEnableVertexAttribArray(semantic::attr::POSITION);
    glEnableVertexAttribArray(semantic::attr::TEXCOORD);

    glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
    glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, 
      GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
    glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, 
      GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);

  // The GL_ARRAY_BUFFER_BINDING is a context state, not a vertex array state.
  glBindBuffer(GL_ARRAY_BUFFER, restoreBuffer);
  glBindVertexArray(restoreVertexArray);

  return vertexArrayName;
}

// OpenGL 4.3 Bind to Create for vertex array object
GLuint CreateVertexArray(GLuint BufferName[])
{
  // Save the previous bound vertex array
  GLuint restoreVertexArray = 0;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);

  GLuint vertexArrayName = 0;
  glGenVertexArrays(1, &vertexArrayName);
  glBindVertexArray(VertexArrayName);
    glEnableVertexAttribArray(semantic::attr::POSITION);
    glEnableVertexAttribArray(semantic::attr::TEXCOORD);

    glVertexAttribBinding(semantic::attr::POSITION, 0);
    glVertexAttribFormat(semantic::attr::POSITION, 2, GL_FLOAT, 
      GL_FALSE, 0);

    glVertexAttribBinding(semantic::attr::TEXCOORD, 0);
    glVertexAttribFormat(semantic::attr::TEXCOORD, 2, GL_FLOAT, 
      GL_FALSE, sizeof(float) * 2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
    glBindVertexBuffer(0, BufferName[buffer::VERTEX], 0, 0);
  glBindVertexArray(restoreVertexArray);

  return vertexArrayName;
}

// Direct State Access
GLuint CreateVertexArray(GLuint BufferName[])
{
  GLuint vertexArrayName = 0;
  glCreateVertexArrays(1, &vertexArrayName);

  glEnableVertexAttribArray(VertexArrayName, semantic::attr::POSITION);
  glEnableVertexAttribArray(VertexArrayName, semantic::attr::TEXCOORD);

  glVertexArrayAttribBinding(VertexArrayName, semantic::attr::POSITION, 0);
  glVertexArrayAttribFormat(VertexArrayName, semantic::attr::POSITION, 2, GL_FLOAT, 
    GL_FALSE, 0);

  glVertexArrayAttribBinding(VertexArrayName, semantic::attr::TEXCOORD, 0);
  glVertexArrayAttribFormat(VertexArrayName, semantic::attr::TEXCOORD, 2, GL_FLOAT, 
    GL_FALSE, sizeof(float) * 2);

  glVertexArrayElementBuffer(VertexArrayName, BufferName[buffer::ELEMENT]);
  glVertexArrayVertexBuffer(VertexArrayName, 0, BufferName[buffer::VERTEX], 0, 0);

  return vertexArrayName;
}
```

## Example 3: Querying the bound texture to a texture image unit for debugging

```cpp
// Select to query
// We need the <target> or we need to loop over all the possible targets
GLuint GetBoundTexture(GLenum target, GLuint unit)
{
  GLuint restore = 0;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &restore);

  glActiveTexture(unit);

  GLuint name = 0;
  glGetIntegerv(target, &name);

  glActiveTexture(restore);
}

// Direct State Access
// target_binding is e.g. GL_TEXTURE_BINDING_2D for the 2D texture
GLuint GetBoundTexture(GLenum target_binding, GLuint unit)
{
  GLuint name = 0;
  glGetIntegeri_v(target_binding, unit, &name);
  return name;
}
```

## Debug Output

L'extension Debug Output ([GL_ARB_debug_output](https://www.opengl.org/registry/specs/ARB/debug_output.txt), puis introduite en Core 4.3) à fait beaucoup de bien aux développeurs OpenGL puisqu'elle fournit un méchanisme de messages d'erreurs bien plus efficace que l'ignoble fonction *glGetError*.

L'idée est de donner une fonction de callback à OpenGL qui sera appelée par l'implémentation dès qu'une erreur est rencontrée. Il est de plus possible de filtrer de manière assez fine les erreurs/warning à ignorer.