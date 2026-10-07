USE vms_db;

CREATE USER IF NOT EXISTS 'vms_app'@'localhost'
IDENTIFIED BY 'Vms2026#Secure';

GRANT SELECT, INSERT, UPDATE, DELETE
ON vms_db.*
TO 'vms_app'@'localhost';

FLUSH PRIVILEGES;

INSERT INTO users(
    username,
    password_hash,
    full_name,
    email,
    is_active
)
VALUES(
    'Gabriel',
    SHA2('gabriel123',256),
    'Gabriel',
    'gabriel@vms.local',
    TRUE
);

INSERT INTO users(
    username,
    password_hash,
    full_name,
    is_active
)
VALUES(
    'operator1',
    SHA2('operator123',256),
    'Operador Principal',
    TRUE
);

INSERT INTO users(
    username,
    password_hash,
    full_name,
    is_active
)
VALUES(
    'viewer1',
    SHA2('viewer123',256),
    'Monitor',
    TRUE
);

INSERT INTO user_roles (user_id, role_id)
SELECT u.id, r.id
FROM users u
JOIN roles r
WHERE u.username = 'Gabriel'
AND r.name = 'admin';

INSERT INTO user_roles (user_id, role_id)
SELECT u.id, r.id
FROM users u
JOIN roles r
WHERE u.username = 'operator1'
AND r.name = 'operator';

INSERT INTO user_roles (user_id, role_id)
SELECT u.id, r.id
FROM users u
JOIN roles r
WHERE u.username = 'viewer1'
AND r.name = 'viewer';

SELECT
    u.username,
    r.name AS rol
FROM users u
JOIN user_roles ur
    ON ur.user_id = u.id
JOIN roles r
    ON r.id = ur.role_id;

SELECT
    id,
    username,
    full_name
FROM users
WHERE username = 'Gabriel'
AND password_hash = SHA2('gabriel123',256)
AND is_active = TRUE;

SELECT p.code
FROM users u
JOIN user_roles ur
    ON ur.user_id = u.id
JOIN role_permissions rp
    ON rp.role_id = ur.role_id
JOIN permissions p
    ON p.id = rp.permission_id
WHERE u.username = 'Gabriel';

SELECT p.code
FROM users u
JOIN user_roles ur
    ON ur.user_id = u.id
JOIN role_permissions rp
    ON rp.role_id = ur.role_id
JOIN permissions p
    ON p.id = rp.permission_id
WHERE u.username = 'operator1';

SELECT p.code
FROM users u
JOIN user_roles ur
    ON ur.user_id = u.id
JOIN role_permissions rp
    ON rp.role_id = ur.role_id
JOIN permissions p
    ON p.id = rp.permission_id
WHERE u.username = 'viewer1';