// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Delayed } from './Delayed';
import { Transform } from './Transform';
import { Pipeline } from './Pipeline';

class Trim implements Transform<string> {
    apply(value: string): string {
        return value.trim();
    }
}

describe('Delayed', () => {
    it('delegates apply() to the constructed transform', () => {
        const d = new Delayed(Trim);
        assert.equal(d.apply('  hello  ', {}), 'hello');
        assert.equal(d.apply('  world  ', {}), 'world');
    });

    it('acquire() applies the transform', () => {
        const d = new Delayed(Trim);
        assert.equal(d.acquire('  hello  ', {}), 'hello');
    });

    it('is assignable to Transform<T>', () => {
        const d: Transform<string> = new Delayed(Trim);
        assert.equal(d.apply('  hi  ', {}), 'hi');
    });

    it('can be installed in a Pipeline', () => {
        const pipeline = new Pipeline<string>();
        pipeline.install(1, 'trim', new Delayed(Trim));
        assert.equal(pipeline.apply('  hello  ', {}), 'hello');
        assert.equal(pipeline.apply('  world  ', {}), 'world');
    });
});
