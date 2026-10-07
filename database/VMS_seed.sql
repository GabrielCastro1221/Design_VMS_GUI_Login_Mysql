USE vms_db;

INSERT INTO roles (name, description) VALUES
    ('admin',    'Acceso total'),
    ('operator', 'Operación diaria'),
    ('viewer',   'Solo visualización');

INSERT INTO permissions (code, description) VALUES
    ('live.view',       'Ver video en vivo'),
    ('playback.view',   'Ver grabaciones'),
    ('export.create',   'Exportar video'),
    ('ptz.control',     'Controlar cámaras PTZ'),
    ('events.view',     'Ver eventos y detecciones'),
    ('events.ack',      'Reconocer eventos'),
    ('cameras.manage',  'Administrar cámaras'),
    ('users.manage',    'Administrar usuarios y roles'),
    ('settings.manage', 'Cambiar la configuración del sistema'),
    ('audit.view',      'Ver el registro de auditoría');

INSERT INTO role_permissions (role_id, permission_id)
SELECT r.id, p.id
FROM roles r CROSS JOIN permissions p
WHERE r.name = 'admin';

INSERT INTO role_permissions (role_id, permission_id)
SELECT r.id, p.id
FROM roles r CROSS JOIN permissions p
WHERE r.name = 'operator'
  AND p.code IN ('live.view','playback.view','export.create','ptz.control','events.view','events.ack');

INSERT INTO role_permissions (role_id, permission_id)
SELECT r.id, p.id
FROM roles r CROSS JOIN permissions p
WHERE r.name = 'viewer'
  AND p.code IN ('live.view','playback.view','events.view');

INSERT INTO event_types (name, description, default_severity) VALUES
    ('motion',          'Movimiento detectado',              'low'),
    ('face_known',      'Rostro conocido reconocido',        'low'),
    ('face_unknown',    'Rostro desconocido',                'medium'),
    ('person',          'Persona detectada',                 'low'),
    ('vehicle',         'Vehículo detectado',                'low'),
    ('camera_offline',  'Cámara sin conexión',               'high'),
    ('tampering',       'Manipulación o tapado de la cámara','critical'),
    ('login_failed',    'Intento de inicio de sesión fallido','medium');